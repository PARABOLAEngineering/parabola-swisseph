// parabola_parallelizer.h
// A universal, type-agnostic thread pool executor for Swiss Ephemeris calls
#pragma once

#include <vector>
#include <future>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>S
#include <functional>
#include "swephexp.h"
extern size_t g_parabola_thread_count;

class ParabolaThreadPool {
public:
    ParabolaThreadPool(size_t threads) {
        for (size_t i = 0; i < threads; ++i) {
            workers.emplace_back([this] {
                while (true) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(mutex);
                        condition.wait(lock, [this] { return stop || !queue.empty(); });
                        if (stop && queue.empty()) return;
                        task = std::move(queue.front());
                        queue.pop();
                    }
                    task();
                }
            });
        }
    }

    template <typename F>
    auto submit(F&& func) -> std::future<typename std::result_of<F()>::type> {
        using R = typename std::result_of<F()>::type;
        auto task = std::make_shared<std::packaged_task<R()>>(std::forward<F>(func));
        std::future<R> res = task->get_future();
        {
            std::unique_lock<std::mutex> lock(mutex);
            queue.emplace([task]() { (*task)(); });
        }
        condition.notify_one();
        return res;
    }

    ~ParabolaThreadPool() {
        {
            std::unique_lock<std::mutex> lock(mutex);
            stop = true;
        }
        condition.notify_all();
        for (auto& t : workers) t.join();
    }

private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> queue;
    std::mutex mutex;
    std::condition_variable condition;
    bool stop = false;
};

// universal parabola<T, R>(inputs, lambda)
template <typename T, typename R>
std::vector<R> parabola(const std::vector<T>& items, std::function<R(const T&)> func) {
    ParabolaThreadPool pool(g_parabola_thread_count);
    std::vector<std::future<R>> futures;
    for (const auto& item : items)
        futures.emplace_back(pool.submit([=]() { return func(item); }));

    std::vector<R> results;
    results.reserve(items.size());
    for (auto& f : futures)
        results.push_back(f.get());
    return results;
}

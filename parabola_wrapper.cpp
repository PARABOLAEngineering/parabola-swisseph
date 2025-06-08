// parabola_wrapper.cpp
// Thread-safe, batch-optimized Swiss Ephemeris parallel executor

#include "parabola_wrapper.h"
#include "swephexp.h"
#include <vector>
#include <string>
#include <future>
#include <mutex>
#include <thread>
#include <queue>
#include <condition_variable>
#include <cstring>
#include <iostream>

// Public API types for use in the rest of Parabola
PlanetBatchResult compute_batch(const PlanetBatchRequest& batch);


size_t g_parabola_thread_count = 1; // default fallback if no autotune is run

class ThreadPool {
public:
    ThreadPool(size_t num_threads) {
        for (size_t i = 0; i < num_threads; ++i) {
            workers.emplace_back([this] {
                while (true) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(queue_mutex);
                        condition.wait(lock, [this] { return stop || !tasks.empty(); });
                        if (stop && tasks.empty()) return;
                        task = std::move(tasks.front());
                        tasks.pop();
                    }
                    task();
                }
            });
        }
    }

    template<class F>
    auto enqueue(F&& f) -> std::future<typename std::invoke_result_t<F>> {
        using return_type = typename std::invoke_result_t<F>;
        auto task = std::make_shared<std::packaged_task<return_type()>>(std::forward<F>(f));
        std::future<return_type> res = task->get_future();
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            tasks.emplace([task]() { (*task)(); });
        }
        condition.notify_one();
        return res;
    }

    ~ThreadPool() {
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            stop = true;
        }
        condition.notify_all();
        for (std::thread &worker : workers)
            worker.join();
    }

private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop = false;
};

size_t autotune_threads(const std::vector<PlanetRequest>& requests) {
    size_t best = 1;
    double best_throughput = 0;
    size_t trial = 1;
    std::vector<PlanetRequest> slice = requests;

    while (true) {
        try {
            auto start = std::chrono::high_resolution_clock::now();
            ThreadPool pool(trial);
            std::vector<std::future<void>> futures;

            size_t slice_size = std::max<size_t>(1, slice.size() / trial);
            for (size_t i = 0; i < slice.size(); i += slice_size) {
                size_t end = std::min(slice.size(), i + slice_size);
                futures.push_back(pool.enqueue([batch = std::vector<PlanetRequest>(slice.begin() + i, slice.begin() + end)]() {
                    for (const auto& req : batch) {
                        double xx[6];
                        char serr[256] = {0};
                        swe_calc_ut(req.jd, req.ipl, SEFLG_SPEED, xx, serr);
                    }
                }));
            }

            for (auto& f : futures) f.get();
            auto end = std::chrono::high_resolution_clock::now();
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
            double throughput = (requests.size() / (double)ms) * 1000.0;

            std::cout << trial << " threads: " << ms << " ms => " << throughput << " planets/sec\n";

            if (throughput > best_throughput) {
                best_throughput = throughput;
                best = trial;
                ++trial;
            } else {
                break; // performance dropped, stop here
            }
        } catch (...) {
            break; // thread creation failed, cap reached
        }
    }
    return best;
}

PlanetBatchResult compute_batch(const PlanetBatchRequest& batch) {
    g_parabola_thread_count = autotune_threads(batch.requests);
    ThreadPool pool(g_parabola_thread_count);
    std::vector<std::future<PlanetBatchResult>> futures;

    size_t slice_size = std::max<size_t>(1, batch.requests.size() / g_parabola_thread_count);

    for (size_t i = 0; i < batch.requests.size(); i += slice_size) {
        size_t end = std::min(batch.requests.size(), i + slice_size);
        std::vector<PlanetRequest> slice(batch.requests.begin() + i, batch.requests.begin() + end);

        futures.emplace_back(pool.enqueue([slice]() -> PlanetBatchResult {
            PlanetBatchResult result;
            for (const auto& req : slice) {
                PlanetResult r = {.ipl = req.ipl};
                std::memset(r.serr, 0, sizeof(r.serr));
                int ret = swe_calc_ut(req.jd, req.ipl, SEFLG_SPEED, r.xx, r.serr);
                r.errcode = ret;
                result.results.push_back(r);
            }
            return result;
        }));
    }

    PlanetBatchResult merged;
    for (auto& fut : futures) {
        PlanetBatchResult r = fut.get();
        merged.results.insert(merged.results.end(), r.results.begin(), r.results.end());
    }

    swe_close();
    return merged;
}

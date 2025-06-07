// swisseph_parallel_patch.cpp
// A standalone patch that wraps Swiss Ephemeris with parallelized batch calls using a thread pool
// Compiles with: g++ -std=c++17 -pthread -lswe -o swisseph_parallel_patch swisseph_parallel_patch.cpp
// On first run, it benchmarks and sets the global thread count for the wrapper

#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include <future>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <cmath>
#include "swephexp.h"
#include "parabola_wrapper.h"
size_t g_parabola_thread_count = 8; // default, gets tuned at runtime

struct PlanetRequest {
    double jd;
    int ipl;
};

struct PlanetResult {
    int ipl;
    double xx[6];
    int errcode;
};

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
    auto enqueue(F&& f) -> std::future<typename std::result_of<F()>::type> {
        using return_type = typename std::result_of<F()>::type;
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

std::vector<PlanetResult> compute_planets_threadpool(const std::vector<PlanetRequest>& requests, size_t num_threads) {
    ThreadPool pool(num_threads);
    std::vector<std::future<PlanetResult>> futures;

    for (const auto& req : requests) {
        futures.emplace_back(pool.enqueue([=]() {
            PlanetResult result = {.ipl = req.ipl};
            char serr[256] = {0};
            swe_calc_ut(req.jd, req.ipl, SEFLG_SPEED, result.xx, serr);
            result.errcode = (serr[0] != '\0');
            return result;
        }));
    }

    std::vector<PlanetResult> results;
    results.reserve(futures.size());
    for (auto& fut : futures) results.push_back(fut.get());
    return results;
}

int swephmain() {
    swe_set_ephe_path("./ephe");

    const double base_jd = 2451545.0; // Jan 1, 2000
    const int planets[] = {SE_SUN, SE_MOON, SE_MERCURY, SE_VENUS, SE_MARS,
                           SE_JUPITER, SE_SATURN, SE_URANUS, SE_NEPTUNE, SE_PLUTO};

    const int charts = 1000;
    std::vector<PlanetRequest> batch;
    for (int i = 0; i < charts; ++i) {
        double jd = base_jd + (i * (1.0 / 1440.0));
        for (int pid : planets) {
            batch.push_back({jd, pid});
        }
    }

    const std::vector<size_t> thread_counts = {1, 2, 4, 8, 16};
    size_t best_threads = 1;
    double best_throughput = 0;

    std::cout << "\n== Autotuning optimal thread count ==\n";
    for (size_t threads : thread_counts) {
        auto start = std::chrono::high_resolution_clock::now();
        auto results = compute_planets_threadpool(batch, threads);
        auto end = std::chrono::high_resolution_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        double throughput = (results.size() / (double)ms) * 1000.0;
        std::cout << threads << " threads: " << ms << " ms => " << throughput << " planets/sec\n";
        if (throughput > best_throughput) {
            best_throughput = throughput;
            best_threads = threads;
        }
    }

    g_parabola_thread_count = best_threads;
    std::cout << "\n[✓] Optimal thread count: " << g_parabola_thread_count << "\n";

    return 0;
}

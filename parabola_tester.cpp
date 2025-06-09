// swisseph_parallel_patch_return.cpp
// High-throughput Swiss Ephemeris parallel chart & house computation with chunked tasks and benchmarking

#include <iostream>
#include <vector>
#include <thread>
#include <future>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <chrono>
#include <numeric>
#include <algorithm>
#include "swephexp.h"

size_t g_parabola_thread_count = 8; // default threads, can be set externally

struct PlanetResult { int ipl; double xx[6]; int errcode; };
struct ChartRequest { double jd, lat, lon; };
struct ChartResult {
    double jd;
    std::vector<PlanetResult> planets;
    double cusps[13];
    double ascmc[10];
};

class ThreadPool {
public:
    ThreadPool(size_t threads) {
        for (size_t i = 0; i < threads; ++i) workers.emplace_back(&ThreadPool::worker_loop, this);
    }

    template<typename F>
    auto enqueue(F&& fn) -> std::future<typename std::result_of<F()>::type> {
        using R = typename std::result_of<F()>::type;
        auto task = std::make_shared<std::packaged_task<R()>>(std::forward<F>(fn));
        std::future<R> fut = task->get_future();
        {
            std::unique_lock<std::mutex> lock(mtx);
            tasks.emplace([task]{ (*task)(); });
        }
        cv.notify_one();
        return fut;
    }

    ~ThreadPool() {
        {
            std::unique_lock<std::mutex> lock(mtx);
            stop = true;
        }
        cv.notify_all();
        for (auto& t : workers) if (t.joinable()) t.join();
    }

private:
    void worker_loop() {
        while (true) {
            std::function<void()> task;
            {
                std::unique_lock<std::mutex> lock(mtx);
                cv.wait(lock, [this]{ return stop || !tasks.empty(); });
                if (stop && tasks.empty()) return;
                task = std::move(tasks.front()); tasks.pop();
            }
            task();
        }
    }
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex mtx;
    std::condition_variable cv;
    bool stop = false;
};

// Compute charts in parallel, chunked into thread_count slices for minimal overhead
std::vector<ChartResult> compute_charts_parallel(const std::vector<ChartRequest>& charts) {
    ThreadPool pool(g_parabola_thread_count);
    size_t n = charts.size();
    size_t threads = g_parabola_thread_count;
    size_t chunk = (n + threads - 1) / threads;
    std::vector<std::future<std::vector<ChartResult>>> futures;
    futures.reserve(threads);

    for (size_t i = 0; i < n; i += chunk) {
        size_t end = std::min(n, i + chunk);
        // capture slice
        futures.emplace_back(pool.enqueue([i, end, &charts]() {
            const int planets[] = {SE_SUN,SE_MOON,SE_MERCURY,SE_VENUS,SE_MARS,
                                   SE_JUPITER,SE_SATURN,SE_URANUS,SE_NEPTUNE,SE_PLUTO};
            std::vector<ChartResult> out;
            out.reserve(end - i);
            for (size_t j = i; j < end; ++j) {
                ChartRequest cr = charts[j];
                ChartResult res; res.jd = cr.jd;
                res.planets.reserve(10);
                // planets
                for (int pid : planets) {
                    PlanetResult pr{pid, {0}, 0};
                    char serr[256] = {0};
                    pr.errcode = swe_calc_ut(cr.jd, pid, SEFLG_SPEED, pr.xx, serr);
                    res.planets.push_back(pr);
                }
                // houses
                int flags = 0;
                swe_houses(cr.jd, cr.lat, cr.lon, flags, res.cusps, res.ascmc);
                out.push_back(std::move(res));
            }
            return out;
        }));
    }
    // collect
    std::vector<ChartResult> all;
    all.reserve(n);
    for (auto& f : futures) {
        auto vec = f.get();
        all.insert(all.end(), std::make_move_iterator(vec.begin()), std::make_move_iterator(vec.end()));
    }
    return all;
}

int main() {
    // path and thread setup
    swe_set_ephe_path("./ephe");
    // optionally tune: g_parabola_thread_count = sysconf(_SC_NPROCESSORS_ONLN);

    const int chart_count = 100; // charts per benchmark
    const int runs = 1000;       // iterations

    // prepare chart inputs
    std::vector<ChartRequest> charts;
    charts.reserve(chart_count);
    double base_jd = 2451545.0;
    for (int i = 0; i < chart_count; ++i)
        charts.push_back({base_jd + i, 52.0, 0.0});

    // warm-up
    compute_charts_parallel(charts);

    // benchmark per run by chart
    using Clock = std::chrono::high_resolution_clock;
    std::vector<double> times; times.reserve(runs);
    for (int r = 0; r < runs; ++r) {
        auto s = Clock::now();
        auto res = compute_charts_parallel(charts);
        auto e = Clock::now();
        times.push_back(std::chrono::duration<double,std::milli>(e-s).count());
    }

    double sum = std::accumulate(times.begin(), times.end(), 0.0);
    double avg = sum / runs;
    double min_t = *std::min_element(times.begin(), times.end());
    double max_t = *std::max_element(times.begin(), times.end());
    double avg_cps = (chart_count / avg) * 1000.0;

    std::cout << "\nBenchmark over " << runs << " runs (" << chart_count << " charts each):\n";
    std::cout << "Avg time: " << avg << " ms => " << avg_cps << " charts/sec\n";
    std::cout << "Best time: " << min_t << " ms => " << (chart_count/min_t*1000.0) << " charts/sec\n";
    std::cout << "Worst time: " << max_t << " ms => " << (chart_count/max_t*1000.0) << " charts/sec\n";

    return 0;
}
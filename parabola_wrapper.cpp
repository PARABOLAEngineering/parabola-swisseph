// parabola_wrapper.cpp
// Production-ready thread-safe Swiss Ephemeris parallel executor

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
#include <atomic>
#include <memory>
#include <algorithm>
// Logging levels
enum class LogLevel { DEBUG, INFO, WARN, ERROR };

// Static initialization control
static std::once_flag init_flag;
static std::atomic<bool> is_initialized{false};
static std::string ephe_path;
static std::mutex logger_mutex;
static std::mutex config_mutex;

// Thread-safe logging
void log_message(LogLevel level, const std::string& message) {
    std::unique_lock<std::mutex> lock(logger_mutex);
    const char* level_str[] = {"[DEBUG] ", "[INFO]  ", "[WARN]  ", "[ERROR] "};
    std::cerr << level_str[static_cast<int>(level)] << message << std::endl;
}

// Singleton ThreadPool with dynamic sizing
class ThreadPool {
public:
    static ThreadPool& instance() {
        static ThreadPool instance(std::thread::hardware_concurrency());
        return instance;
    }

    size_t size() const {
        return workers.size();
    }

    void resize(size_t num_threads) {
        if (num_threads == 0) {
            num_threads = std::thread::hardware_concurrency();
        }

        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;
        condition.notify_all();
        for (std::thread& worker : workers) {
            if (worker.joinable()) {
                worker.join();
            }
        }

        workers.clear();
        stop = false;
        for (size_t i = 0; i < num_threads; ++i) {
            workers.emplace_back([this, i] {
                              swe_set_tid_acc(i); // Unique thread ID for Swiss Ephemeris
                                worker_loop();
            });
        }
    }

    template<class F>
    auto enqueue(F&& f) -> std::future<typename std::invoke_result_t<F>> {
        using return_type = typename std::invoke_result_t<F>;
        
        auto task = std::make_shared<std::packaged_task<return_type()>>(
            [f = std::forward<F>(f)]() mutable {
                try {
                    return f();
                } catch (const std::exception& e) {
                    log_message(LogLevel::ERROR, std::string("Task failed: ") + e.what());
                    throw;
                }
            });

        std::future<return_type> res = task->get_future();
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            if (stop) {
                throw std::runtime_error("Enqueue on stopped ThreadPool");
            }
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
        for (std::thread& worker : workers) {
            if (worker.joinable()) {
                worker.join();
            }
        }
        swe_close();
    }

private:
    ThreadPool(size_t num_threads) {
        resize(num_threads);
    }

    void worker_loop() {
        while (true) {
            std::function<void()> task;
            {
                std::unique_lock<std::mutex> lock(queue_mutex);
                condition.wait(lock, [this] { return stop || !tasks.empty(); });
                if (stop && tasks.empty()) {
                    return;
                }
                task = std::move(tasks.front());
                tasks.pop();
            }
            task();
        }
    }

    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop = false;
};

// Initialize Swiss Ephemeris once
void initialize_swiss_ephemeris(const std::string& path) {
    std::call_once(init_flag, [&path]() {
        ephe_path = path;
        swe_set_ephe_path(ephe_path.c_str());
        
        // Verify ephemeris files are accessible
        char serr[256] = {0};
        double xx[6];
        if (swe_calc_ut(2451545.0, SE_SUN, SEFLG_SPEED, xx, serr) < 0) {
            log_message(LogLevel::ERROR, std::string("Ephemeris initialization failed: ") + serr);
            throw std::runtime_error("Ephemeris initialization failed");
        }
        
        is_initialized = true;
        log_message(LogLevel::INFO, "Swiss Ephemeris initialized successfully");
    });
}

// Create representative workload for autotuning
std::vector<PlanetRequest> create_test_workload(size_t count = 1000) {
    std::vector<PlanetRequest> workload;
    const double base_jd = 2451545.0; // Jan 1, 2000
    const int planets[] = {SE_SUN, SE_MOON, SE_MERCURY, SE_VENUS, SE_MARS,
                          SE_JUPITER, SE_SATURN, SE_URANUS, SE_NEPTUNE, SE_PLUTO};

    workload.reserve(count * std::size(planets));
    for (size_t i = 0; i < count; ++i) {
        double jd = base_jd + (i * (1.0 / 1440.0));
        for (int pid : planets) {
            workload.push_back({jd, pid});
        }
    }
    return workload;
}

// Autotune thread count (runs once at startup)
size_t autotune_threads(size_t max_threads = 0) {
    if (!is_initialized) {
        throw std::runtime_error("Swiss Ephemeris not initialized");
    }

    const auto test_workload = create_test_workload();
    if (max_threads == 0) {
        max_threads = std::thread::hardware_concurrency() * 2;
    }

    size_t best_threads = 1;
    double best_throughput = 0;

    log_message(LogLevel::INFO, "Starting thread autotuning...");

    for (size_t threads = 1; threads <= max_threads; threads = (threads < 4) ? threads + 1 : threads * 2) {
        try {
            auto& pool = ThreadPool::instance();
            pool.resize(threads);

            auto start = std::chrono::high_resolution_clock::now();
            std::vector<std::future<void>> futures;

            size_t slice_size = std::max<size_t>(1, test_workload.size() / threads);
            for (size_t i = 0; i < test_workload.size(); i += slice_size) {
                size_t end = std::min(test_workload.size(), i + slice_size);
                futures.push_back(pool.enqueue([slice = std::vector<PlanetRequest>(
                    test_workload.begin() + i, test_workload.begin() + end)]() {
                    for (const auto& req : slice) {
                        double xx[6];
                        char serr[256] = {0};
                        int ret = swe_calc_ut(req.jd, req.ipl, SEFLG_SPEED, xx, serr);
                        if (ret < 0) {
                            log_message(LogLevel::WARN, 
                                std::string("Calculation error during tuning: ") + serr);
                        }
                    }
                }));
            }

            for (auto& f : futures) f.get();
            auto end = std::chrono::high_resolution_clock::now();
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
            double throughput = (test_workload.size() / (double)ms) * 1000.0;

            log_message(LogLevel::INFO, 
                std::to_string(threads) + " threads: " + 
                std::to_string(ms) + " ms => " + 
                std::to_string(throughput) + " planets/sec");

            if (throughput > best_throughput * 1.05) { // 5% improvement threshold
                best_throughput = throughput;
                best_threads = threads;
            } else if (threads > best_threads && throughput >= best_throughput * 0.95) {
                best_threads = threads; // Prefer more threads if performance is similar
            }
        } catch (...) {
            break; // Stop if thread creation fails
        }
    }

    log_message(LogLevel::INFO, 
        "Optimal thread count: " + std::to_string(best_threads));
    return best_threads;
}

// Main batch computation function
PlanetBatchResult compute_batch(const PlanetBatchRequest& batch) {
    if (!is_initialized) {
        throw std::runtime_error("Swiss Ephemeris not initialized");
    }

    auto& pool = ThreadPool::instance();
    std::vector<std::future<PlanetBatchResult>> futures;

    // Dynamic batching - smaller batches for better load balancing
    const size_t min_batch_size = 10;
    const size_t max_batch_size = 100;
    size_t target_batch_size = std::min(
        max_batch_size,
        std::max(min_batch_size, batch.requests.size() / pool.size()));

    for (size_t i = 0; i < batch.requests.size(); i += target_batch_size) {
        size_t end = std::min(batch.requests.size(), i + target_batch_size);
        std::vector<PlanetRequest> slice(batch.requests.begin() + i, batch.requests.begin() + end);

        futures.emplace_back(pool.enqueue([slice]() -> PlanetBatchResult {
            PlanetBatchResult result;
            result.results.reserve(slice.size());

            for (const auto& req : slice) {
                PlanetResult r = {.ipl = req.ipl};
                std::memset(r.serr, 0, sizeof(r.serr));
                r.errcode = swe_calc_ut(req.jd, req.ipl, SEFLG_SPEED, r.xx, r.serr);
                
                if (r.errcode < 0) {
                    log_message(LogLevel::WARN, 
                        std::string("Calculation error for planet ") + 
                        std::to_string(req.ipl) + " at JD " + 
                        std::to_string(req.jd) + ": " + r.serr);
                }
                
                result.results.push_back(r);
            }
            return result;
        }));
    }

    PlanetBatchResult merged;
    size_t total_results = 0;
    for (auto& fut : futures) {
        PlanetBatchResult r = fut.get();
        merged.results.insert(merged.results.end(), 
            std::make_move_iterator(r.results.begin()),
            std::make_move_iterator(r.results.end()));
        total_results += r.results.size();
    }

    if (total_results != batch.requests.size()) {
        log_message(LogLevel::ERROR, 
            "Result count mismatch: expected " + 
            std::to_string(batch.requests.size()) + ", got " + 
            std::to_string(total_results));
        throw std::runtime_error("Result count mismatch");
    }

    return merged;
}

// Initialization API
// Just initialization - no tuning
// Updated initialization API
void initialize(const std::string& ephemeris_path, size_t thread_count) {
    initialize_swiss_ephemeris(ephemeris_path);
    
    // Basic validation
    if (thread_count == 0) {
        thread_count = std::thread::hardware_concurrency();
        log_message(LogLevel::INFO, 
            "Using hardware concurrency: " + std::to_string(thread_count));
    }
    
    ThreadPool::instance().resize(thread_count);
}

// Standalone tuning executable
int main(int argc, char** argv) {
    if (argc > 1 && std::string(argv[1]) == "--tune") {
        std::string ephe_path = "./ephe";
        std::string config_path;
        
        // Parse args (simulation for benchmarking)
        if (argc > 2) ephe_path = argv[2];
        if (argc > 3) config_path = argv[3];
        
        try {
                    initialize_swiss_ephemeris(ephe_path);
                    size_t threads = autotune_threads();
                    std::cout << "Optimal thread count: " << threads << std::endl;
                    return 0;
                } catch (...) {
                    return 1;
                }
            }
        }
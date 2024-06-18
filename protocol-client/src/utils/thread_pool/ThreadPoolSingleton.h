#pragma once
#include "./thread_pool.h"

class ThreadPoolSingleton {
public:
    inline static thread_pool::ThreadPool pool {std::thread::hardware_concurrency() - 1 };
private:
    ThreadPoolSingleton() = default;
    ~ThreadPoolSingleton() = default;
    ThreadPoolSingleton(const ThreadPoolSingleton &) = delete;
    ThreadPoolSingleton &operator=(const ThreadPoolSingleton &) = delete;
};

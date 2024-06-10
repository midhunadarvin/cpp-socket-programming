#pragma once
#include <queue>
#include <mutex>

template <typename T>
class ThreadSafeQueue {
public:
    // Constructor
    ThreadSafeQueue() {}

    // Destructor
    ~ThreadSafeQueue() {}

    // Push an element to the queue
    void push(const T& value) {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push(value);
    }

    // Pop an element from the queue
    // Note: This function assumes that the queue is not empty.
    T pop() {
        std::lock_guard<std::mutex> lock(mutex_);

        if (queue_.size() == 0) return NULL; 
        T value = queue_.front();
        queue_.pop();
        return value;
    }

    // Check if the queue is empty
    bool empty() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.empty();
    }

    // Get the size of the queue
    size_t size() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.size();
    }

private:
    std::queue<T> queue_;
    mutable std::mutex mutex_;
};

// Example usage:
// ThreadSafeQueue<int> threadSafeQueue;
// threadSafeQueue.push(42);
// int value = threadSafeQueue.pop();
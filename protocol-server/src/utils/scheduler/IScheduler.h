#pragma once
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <functional>

// Task structure to hold task information
struct ITask {
    std::string name;
    std::chrono::system_clock::time_point time;
    std::function<void()> function;
};

// Scheduler class to manage tasks
class IScheduler {
protected:
    std::vector<ITask> tasks;

public:
    virtual ~IScheduler() = default;

    // Function to add a task
    virtual void addTask(
        const std::string& name, 
        const std::chrono::system_clock::time_point& time, 
        const std::function<void()>& func
    ) = 0;

    // Function to start executing tasks
    virtual void start() = 0;

    // Function to stop the scheduler
    virtual void stop() = 0;
};

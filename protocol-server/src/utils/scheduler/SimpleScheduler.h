#pragma once
#include "IScheduler.h"

// Scheduler class to manage tasks
class SimpleScheduler : public IScheduler {
private:
    bool running;

public:
    SimpleScheduler() : running(true) {}

    // Function to add a task
    void addTask(
        const std::string& name, 
        const std::chrono::system_clock::time_point& time, 
        const std::function<void()>& func
    ) override;

    // Function to start executing tasks
    void start() override;

    // Function to stop the scheduler
    void stop() override;
};

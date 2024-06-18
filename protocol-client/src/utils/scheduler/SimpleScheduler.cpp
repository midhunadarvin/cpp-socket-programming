#include "SimpleScheduler.h"

// Function to add a task
void SimpleScheduler::addTask(const std::string &name,
                              const std::chrono::system_clock::time_point &time,
                              const std::function<void()> &func) {
  tasks.push_back({name, time, func});
}

// Function to start executing tasks
void SimpleScheduler::start() {
  while (running) {
    auto now = std::chrono::system_clock::now();
    for (auto it = tasks.begin(); it != tasks.end();) {
      if (it->time <= now) {
        std::cout << "Executing task: " << it->name << std::endl;
        it->function();
        it = tasks.erase(it);
      } else {
        ++it;
      }
    }
    std::this_thread::sleep_for(
        std::chrono::milliseconds(1000)); // Check every second
  }
}

void SimpleScheduler::stop() { running = false; }

// Task structure to hold task information
struct Task {
  std::string name;
  std::chrono::system_clock::time_point time;
  std::function<void()> function;
};

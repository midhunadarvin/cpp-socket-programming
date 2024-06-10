#include "./thread_pool.h"

namespace thread_pool {
auto make_thread_handler(
    thread_pool::ThreadSafeQueue<thread_pool::Task> &queue) {
  return std::thread{[&queue] {
    while (true) {
      auto const elem = queue.pop();
      switch (elem.type) {
      case thread_pool::TaskType::Execute:
        elem.task(elem.arguments);
        break;
      case thread_pool::TaskType::Stop:
        return;
      }
    }
  }};
}
} // namespace thread_pool

thread_pool::ThreadPool::ThreadPool(std::size_t n_threads) {
  for (std::size_t i = 0; i < n_threads; ++i) {
    _threads.push_back(make_thread_handler(_queue));
  }
}

thread_pool::ThreadPool::~ThreadPool() {
  Task const stop_task{TaskType::Stop, {}, {}};
  for (std::size_t i = 0; i < _threads.size(); ++i) {
    push(stop_task);
    if (_threads[i].joinable()) {
      _threads[i].join();
    }
  }
}

bool thread_pool::ThreadPool::push(Task const &task) {
  _queue.push(task);
  return true;
}

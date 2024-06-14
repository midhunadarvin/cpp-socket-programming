#include "SchedulerHandler.h"
#include "../engine/constants.h"
#include "../utils/thread_pool/ThreadPoolSingleton.h"

/**
 * Function that handles data
 * @param request_string - the buffer that we receive from client
 * @param buffer_length - length of the buffer
 */
std::string SchedulerHandler::HandleData(std::string buffer,
                                         unsigned long buffer_length,
                                         EXECUTION_CONTEXT *exec_context) {
  std::string request_string = buffer;
  request_string = request_string.substr(0, buffer_length);

  ThreadPoolSingleton::pool.push({
      thread_pool::TaskType::Execute, // TaskType
      [](std::vector<thread_pool::Param> const &params) {
        std::cout << "Task 1 executed!" << std::endl;
      },
      {} // Arguments
  });

  return "HTTP/1.1 200 OK\r\n";
}

#include "SyncApiProtocolHandler.h"
#include "../engine/constants.h"
#include "../utils/logger/Logger.h"
#include "../payload/Payload.h"

/**
 * Function that handles data
 * @param request_string - the buffer that we receive from client
 * @param buffer_length - length of the buffer
 */
std::string SyncApiProtocolHandler::HandleData(std::string buffer,
                                         unsigned long buffer_length,
                                         EXECUTION_CONTEXT *exec_context) {
  std::string request_string = buffer;
  request_string = request_string.substr(0, buffer_length);

  PAYLOAD* payload =  (PAYLOAD *) request_string.c_str();

  LOG_INFO(std::to_string(payload->id));
  LOG_INFO(payload->name);
    
  return "HTTP/1.1 200 OK\r\n";
}

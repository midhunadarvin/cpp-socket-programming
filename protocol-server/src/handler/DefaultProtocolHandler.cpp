#include "DefaultProtocolHandler.h"
#include "../utils/logger/Logger.h"
#include "../engine/constants.h"

/**
 * Function that handles data
 * @param request_string - the buffer that we receive from client
 * @param buffer_length - length of the buffer
 */
std::string DefaultProtocolHandler::HandleData(std::string buffer, unsigned long buffer_length, EXECUTION_CONTEXT *exec_context) {
    std::string request_string = buffer;
    request_string = request_string.substr(0, buffer_length);
    return "HTTP/1.1 200 OK\r\n";
}

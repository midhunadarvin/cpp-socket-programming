#include "SyncApiProtocolHandler.h"
#include "../engine/constants.h"
#include "../payload/Payload.h"
#include "../utils/logger/Logger.h"
#include "../utils/thread_pool/ThreadPoolSingleton.h"
#include "../utils/uuid/UuidGenerator.h"
#include <stdio.h>
#include <string>
#include <unistd.h>

/**
 * Function that handles data
 * @param request_string - the buffer that we receive from client
 * @param buffer_length - length of the buffer
 */
std::string
SyncApiProtocolHandler::HandleData(std::string buffer,
                                   unsigned long buffer_length,
                                   EXECUTION_CONTEXT *exec_context) {
  std::string request_string = buffer;
  request_string = request_string.substr(0, buffer_length);

  int packet_type = request_string[0];

  if (packet_type == PacketType::CONNECT) {
    CONNECT_PACKET *packet = (CONNECT_PACKET *)request_string.c_str();
    auto correlation_id = UuidGenerator::generateUuid();

    LOG_INFO(packet->action);
    LOG_INFO(std::to_string(sizeof(correlation_id)));

    ThreadPoolSingleton::pool.push({
        thread_pool::TaskType::Execute, // TaskType
        [packet, correlation_id,
         this](std::vector<thread_pool::Param> const &params) {
          // sdk call (mock)
          sleep(10);

          // update map
          this->response_map[correlation_id] = "Response Received";
          std::cout << "Task " << packet->action << " executed!" << std::endl;
        },
        {} // Arguments
    });

    this->response_map[correlation_id] = "";

    // send back connect response packet
    CONNECT_RESPONSE_PACKET response_packet =
        MakeConnectResponse((char *)correlation_id.c_str());
    std::cout << response_packet.packet_type << std::endl;
    std::cout << response_packet.flag << std::endl;
    std::cout << response_packet.task_id << std::endl;
    std::cout << sizeof(response_packet) << " "
              << sizeof((char *)correlation_id.c_str()) << std::endl;

    return std::string((char *)&response_packet, sizeof(response_packet));
  } else if (packet_type == PacketType::CHECK_STATUS) {
    CHECK_STATUS_PACKET *check_status_packet =
        (CHECK_STATUS_PACKET *)request_string.c_str();
    LOG_INFO(check_status_packet->task_id);

    CHECK_STATUS_RESPONSE_PACKET check_status_response_packet;
    check_status_response_packet.packet_type = 4;

    if (this->response_map[check_status_packet->task_id] == "") {
      strcpy(check_status_response_packet.status, "pending");
    } else {
      strcpy(check_status_response_packet.status, "completed");
      strcpy(check_status_response_packet.response,
             response_map[check_status_packet->task_id].c_str());
    }

    return std::string((char *)&check_status_response_packet,
                       sizeof(check_status_response_packet));
  }

  return "HTTP/1.1 200 OK\r\n";
}

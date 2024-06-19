#include "utils/logger/Logger.h"
#include "engine/socket/CClientSocket.h"
#include "engine/constants.h"
#include <unistd.h>
#include "payload/Payload.h"
#include <unistd.h>

int main(int argc, char const *argv[]) {
	LOG_INFO("Hello world!");

	CONNECT_PACKET connect_packet {
		1,
		"test"
	};

	END_POINT target_endpoint {
        "localhost", 
		8080, 
		0, 
		"", 
		0, 
		""
	};

    LOG_INFO("Resolved (Target) Host: " + target_endpoint.ipaddress);
    LOG_INFO("Resolved (Target) Port: " + std::to_string(target_endpoint.port));


	CClientSocket *target_socket;
    try {
        target_socket = new CClientSocket(target_endpoint.ipaddress, target_endpoint.port);
    } catch (std::exception &e) {
        std::cout << e.what() << std::endl;
        LOG_ERROR(std::string("Error connecting to target : ") + e.what());
		exit(1);
    }

	target_socket->SendBytes((void *) &connect_packet, sizeof(connect_packet));

	std::string response = target_socket->ReceiveBytes();
	CONNECT_RESPONSE_PACKET* response_packet = (CONNECT_RESPONSE_PACKET *) response.c_str();
	std::string task_id = std::string(response_packet->task_id);
	delete target_socket;

	bool responseReceived = false;
	while (!responseReceived) {
		sleep(2);
		LOG_INFO("Checking for status ...");
		try {
			target_socket = new CClientSocket(target_endpoint.ipaddress, target_endpoint.port);
		} catch (std::exception &e) {
			std::cout << e.what() << std::endl;
			LOG_ERROR(std::string("Error connecting to target : ") + e.what());
			exit(1);
		}
		CHECK_STATUS_PACKET check_status_packet;
		check_status_packet.packet_type = 3;
		std::strcpy(check_status_packet.task_id, task_id.c_str());
		target_socket->SendBytes((void *) &check_status_packet, sizeof(check_status_packet));

		std::string response = target_socket->ReceiveBytes();
		CHECK_STATUS_RESPONSE_PACKET* response_packet = (CHECK_STATUS_RESPONSE_PACKET *) response.c_str();
		LOG_INFO(response_packet->status);
		delete target_socket;
		if (std::strcmp(response_packet->status,"completed") == 0) {
			responseReceived = true;
			break;
			LOG_INFO(response_packet->response);
		}
	}

	return 0;
}

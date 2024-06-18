#include "utils/logger/Logger.h"
#include "engine/socket/CClientSocket.h"
#include "engine/constants.h"
#include <unistd.h>
#include "payload/Payload.h"

int main(int argc, char const *argv[]) {
	LOG_INFO("Hello world!");

	PAYLOAD sample {
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

	target_socket->SendBytes((void *) &sample, sizeof(sample));

	std::string response = target_socket->ReceiveBytes();
	LOG_INFO(response);

	pause();
	return 0;
}

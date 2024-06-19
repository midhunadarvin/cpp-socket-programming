#include "CProtocolPipeline.h"
#include "../payload/Payload.h"

extern "C" void *CProtocolPipeline(CProtocolSocket *ptr, void *lptr) {
    LOG_INFO("Starting ProtocolPipeline");
    CLIENT_DATA clientData;
    memcpy(&clientData, lptr, sizeof(CLIENT_DATA));

    // Check if handler is defined
    CProtocolHandler *protocol_handler = ptr->GetHandler();
    if (protocol_handler == nullptr) {
        LOG_ERROR("The handler is not defined. Exiting!");
        return nullptr;
    }
    auto *client_socket = (Socket *) clientData.client_socket;
    EXECUTION_CONTEXT exec_context;

    ProtocolHelper::SetReadTimeOut(client_socket->GetSocket(), 1);
    std::string request;
    std::string response;
    int retry = 0;
    while (true) {
        try {
			SocketSelect sel = SocketSelect(client_socket, nullptr, NonBlockingSocket);

			bool still_connected = true;

			try {
				if (sel.Readable(client_socket)) {
					LOG_INFO("client socket is readable, reading bytes : ");
					std::string bytes = client_socket->ReceiveBytes();

					if (!bytes.empty()) {
						LOG_INFO("Calling Protocol Handler..");
						request += bytes;
					}
					if (bytes.empty())
						still_connected = false;
				} else {
					retry++;
					if (retry == 100) {
						still_connected = false;
					}
				}
			}
			catch (std::exception &e) {
				LOG_ERROR("Error while sending to target :");
				LOG_ERROR(e.what());
			}

			if (!still_connected) {
				break;
			}
        }
        catch (std::exception &e) {
            LOG_ERROR(e.what());
            LOG_ERROR("error occurred while creating socket select ");
        }
    }

    if (!request.empty()) {
        response = protocol_handler->HandleData(request, request.size(), &exec_context);
        std::cout << response.c_str() << std::endl;
        CONNECT_RESPONSE_PACKET * packet = (CONNECT_RESPONSE_PACKET *) response.c_str();
        LOG_INFO(packet->task_id);
        client_socket->SendBytes((char *) response.c_str(), response.size());
    }

    // Close the client socket
    LOG_INFO("Closing the client socket");
    client_socket->Close();

#ifdef WINDOWS_OS
    return 0;
#else
    return nullptr;
#endif
}
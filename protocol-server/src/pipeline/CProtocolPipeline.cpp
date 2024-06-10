#include "CProtocolPipeline.h"

extern "C" void *CProtocolPipeline(CProtocolSocket *ptr, void *lptr) {
    printf("Starting ProtocolPipeline");
    CLIENT_DATA clientData;
    memcpy(&clientData, lptr, sizeof(CLIENT_DATA));

    // Check if handler is defined
    CProtocolHandler *protocol_handler = ptr->GetHandler();
    if (protocol_handler == nullptr) {
        printf("The handler is not defined. Exiting!");
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
					printf("client socket is readable, reading bytes : ");
					std::string bytes = client_socket->ReceiveBytes();

					if (!bytes.empty()) {
						printf("Calling Protocol Handler..");
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
				printf("Error while sending to target :");
				printf("%s", e.what());
			}

			if (!still_connected) {
				break;
			}
        }
        catch (std::exception &e) {
            printf("%s", e.what());
            printf("error occurred while creating socket select ");
        }
    }

    if (!request.empty()) {
        response = protocol_handler->HandleData(request, request.size(), &exec_context);
        client_socket->SendBytes((char *) response.c_str(), response.size());
    }

    // Close the client socket
    printf("Closing the client socket");
    client_socket->Close();

#ifdef WINDOWS_OS
    return 0;
#else
    return nullptr;
#endif
}
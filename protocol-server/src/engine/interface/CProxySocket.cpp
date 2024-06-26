#include "CProxySocket.h"
#include "../socket/CClientSocket.h"
#include "../../utils/ProtocolHelper.h"
#include "../socket/SocketSelect.h"

void *CProxySocket::ThreadHandler(CProxySocket *ptr, void *lptr)
{
    CLIENT_DATA clientData;
    memcpy(&clientData, lptr, sizeof(CLIENT_DATA));

    // Retrieve the load balancer class
    auto loadBalancer = ptr->loadBalancer;
    
    // Check if handler is defined
    CProxyHandler *proxy_handler = ptr->GetHandler();
    if (proxy_handler == 0)
    {
        std::cout << "The handler is not defined. Exiting!" << std::endl;
        return 0;
    }

    RESOLVED_SERVICE currentService = loadBalancer->requestServer();
    END_POINT target_endpoint {
        currentService.ipaddress, currentService.port, currentService.r_w, currentService.alias, currentService.reserved, "  "
    };

    std::cout << "Resolved (Target) Host: " << target_endpoint.ipaddress << std::endl
         << "Resolved (Target) Port: " << target_endpoint.port << std::endl;

    auto *client_socket = (Socket *)clientData.client_socket;
    auto *target_socket = new CClientSocket(target_endpoint.ipaddress, target_endpoint.port);

    EXECUTION_CONTEXT exec_context;

    ProtocolHelper::SetReadTimeOut(client_socket->GetSocket(), 1);
    ProtocolHelper::SetKeepAlive(client_socket->GetSocket(), 1);
    ProtocolHelper::SetReadTimeOut(target_socket->GetSocket(), 1);
    ProtocolHelper::SetKeepAlive(client_socket->GetSocket(), 1);

    while (true)
    {
        SocketSelect *sel;
        try
        {
            sel = new SocketSelect(client_socket, target_socket, NonBlockingSocket);
        }
        catch (std::exception &e)
        {
            std::cout << e.what() << std::endl;
            std::cout << "error occurred while creating socket select " << std::endl;
        }

        bool still_connected = true;
        try
        {
            if (sel->Readable(client_socket))
            {
                std::cout << "client socket is readable, reading bytes : " << std::endl;
                std::string bytes = client_socket->ReceiveBytes();

                std::cout << "Calling Proxy Upstream Handler.." << std::endl;
                std::string response = proxy_handler->HandleUpstreamData(bytes, bytes.size(), &exec_context);
                target_socket->SendBytes((char *)response.c_str(), response.size());

                if (bytes.empty())
                    still_connected = false;
            }
        }
        catch (std::exception &e)
        {
            std::cout << "Error while sending to target " << e.what() << std::endl;
        }

        try
        {
            if (sel->Readable(target_socket))
            {
                std::cout << "target socket is readable, reading bytes : " << std::endl;
                std::string bytes = target_socket->ReceiveBytes();

                std::cout << "Calling Proxy Downstream Handler.." << std::endl;
                std::string response = proxy_handler->HandleDownStreamData(bytes, bytes.size(), &exec_context);
                client_socket->SendBytes((char *)response.c_str(), response.size());

                if (bytes.empty())
                    still_connected = false;
            }
        }
        catch (std::exception &e)
        {
            std::cout << "Error while sending to client " << e.what() << std::endl;
        }

        if (!still_connected)
        {
            break;
        }
    }
#ifdef WINDOWS_OS
    return 0;
#else
    return nullptr;
#endif
}
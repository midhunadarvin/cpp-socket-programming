/*
   proxy.cpp
*/

// Build : g++ proxy.cpp -o proxy.exe
// Run   : ./Proxy.exe <proxy-port> <server-host> <server-port>

#include "socket/Socket.h"
#include "socket/CClientSocket.h"
#include "socket/CServerSocket.h"
#include "socket/SocketSelect.h"
#include <iostream> // standard input-output stream, contains utilities for dealing with input / output and streams
#ifdef WINDOWS_OS
#include <process.h> // contains function declarations and mappings for threading interfaces and defines a number of constants used by those functions. ( windows )
#else
#include <pthread.h> // contains function declarations and mappings for threading interfaces and defines a number of constants used by those functions.
#endif

#include <sys/select.h>
#include "proxy.h"

/**
 * The thread method that will handle exchange of requests between
 * the client and the server
 */
#ifdef WINDOWS_OS
unsigned __stdcall Proxy::RunProxyClientThread(void *a)
#else
void *Proxy::RunProxyClientThread(void *a)
#endif
{
    Socket *client_socket = (Socket *)a;
    // CClientSocket target_socket(server_addr, server_port);
    ConnectionPool::ConnectionProxy connection = pool->get_connection();
    if (!connection.valid())
    {
        throw std::runtime_error("Unable to fetch connection from pool");
    }
    else
    {
        std::cout << "Got connection from pool :: " << std::endl;
    }
    auto &tcp_connection = dynamic_cast<TcpConnection &>(*connection);
    std::cout << tcp_connection.connection_id << std::endl;
    CClientSocket *target_socket = tcp_connection.client_socket;
    while (true)
    {
        SocketSelect *sel;
        try
        {
            sel = new SocketSelect(client_socket, target_socket, NonBlockingSocket);
        }
        catch (std::exception &e)
        {
            cout << e.what() << endl;
            cout << "error occurred while creating socket select " << endl;
            // Close the client socket
            std::cout << "Closing the client socket" << std::endl;
            client_socket->Close();
            delete client_socket;
            client_socket = nullptr;
            break;
        }

        bool still_connected = true;
        try
        {
            if (sel->Readable(client_socket))
            {
                cout << "client socket is readable, reading bytes : " << endl;
                std::string bytes = client_socket->ReceiveBytes();

//                cout << "Calling Proxy Upstream Handler.." << endl;
                // std::string response = proxy_handler->HandleUpstreamData((void *)bytes.c_str(), bytes.size(), &exec_context);
                target_socket->SendBytes((char *)bytes.c_str(), bytes.size());

                if (bytes.empty())
                    still_connected = false;
            }
        }
        catch (std::exception &e)
        {
            cout << "Error while sending to target " << e.what() << endl;
        }

        try
        {
            if (sel->Readable(target_socket))
            {
                cout << "target socket is readable, reading bytes : " << endl;
                std::string bytes = target_socket->ReceiveBytes();

//                cout << "Calling Proxy Downstream Handler.." << endl;
                // std::string response = proxy_handler->HandleDownStreamData((void *)bytes.c_str(), bytes.size(), &exec_context);
                client_socket->SendBytes((char *)bytes.c_str(), bytes.size());

                if (bytes.empty())
                    still_connected = false;
            }
        }
        catch (std::exception &e)
        {
            cout << "Error while sending to client " << e.what() << endl;
        }

        if (!still_connected)
        {
            // Close the client socket
             std::cout << "Closing the client socket" << std::endl;
            client_socket->Close();
            delete client_socket;
            client_socket = nullptr;
            break;
        }
    }

    // Close the server socket
    std::cout << "Closing the target socket" << std::endl;
//    target_socket->Close();
//    delete target_socket;
//    target_socket = nullptr;
#ifdef WINDOWS_OS
    return 0;
#else
    return nullptr;
#endif
}
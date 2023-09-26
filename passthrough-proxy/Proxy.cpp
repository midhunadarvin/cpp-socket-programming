/*
   proxy.cpp

   Copyright (C) 2002-2017 René Nyffenegger

   This source code is provided 'as-is', without any express or implied
   warranty. In no event will the author be held liable for any damages
   arising from the use of this software.

   Permission is granted to anyone to use this software for any purpose,
   including commercial applications, and to alter it and redistribute it
   freely, subject to the following restrictions:

   1. The origin of this source code must not be misrepresented; you must not
      claim that you wrote the original source code. If you use this source code
      in a product, an acknowledgment in the product documentation would be
      appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not be
      misrepresented as being the original source code.

   3. This notice may not be removed or altered from any source distribution.

   René Nyffenegger rene.nyffenegger@adp-gmbh.ch
*/

// Build : g++ Proxy.cpp Socket.cpp -o Proxy.exe
// Run   : ./Proxy.exe <proxy-port> <server-host> <server-port>

#include "Socket.h"

#include <iostream>   // standard input-output stream, contains utilities for dealing with input / output and streams
#ifdef WINDOWS_OS
#include <process.h>  // contains function declarations and mappings for threading interfaces and defines a number of constants used by those functions. ( windows )
#else
#include <pthread.h>  // contains function declarations and mappings for threading interfaces and defines a number of constants used by those functions.
#endif
#include <sstream>    // provides templates and types that enable interoperation between stream buffers and string objects
#include <sys/select.h>

int proxy_port;
std::string server_addr;
int server_port;

/**
 * The thread method that will handle exchange of requests between 
 * the client and the server
*/
#ifdef WINDOWS_OS
unsigned __stdcall RunProxyThread(void *a)
#else
void *RunProxyThread(void *a)
#endif
{
    Socket *client_socket = (Socket *)a;
    SocketClient target_socket(server_addr, server_port);

    while (1)
    {
        SocketSelect sel(client_socket, &target_socket, NonBlockingSocket);

        bool still_connected = true;

        if (sel.Readable(client_socket))
        {
            std::string bytes = client_socket->ReceiveBytes();
            target_socket.SendBytes(bytes);
            std::cout << "Server: " << bytes << std::endl;
            if (bytes.empty())
                still_connected = false;
        }
        if (sel.Readable(&target_socket))
        {
            std::string bytes = target_socket.ReceiveBytes();
            client_socket->SendBytes(bytes);
            std::cout << "Client: " << bytes << std::endl;
            if (bytes.empty())
                still_connected = false;
        }
        if (!still_connected)
        {
            break;
        }
    }

    delete client_socket;
#ifdef WINDOWS_OS
    return 0;
#else
    return nullptr;
#endif
}

int main(int argc, char *argv[])
{
    if (argc < 4)
    {
        std::cout << "Usage:" << std::endl;
        std::cout << "  proxy <port proxy> <addr server> <port server>" << std::endl;
        std::cout << std::endl;
        std::cout << "  This proxy will then listen on <port proxy> and whenever it receives" << std::endl;
        std::cout << "  a connection, relays the traffic to the <port server> of <addr server>." << std::endl;
        std::cout << "  This makes it ideal to see what an SMTP Client exchanges with a SMTP Server," << std::endl;
        std::cout << "  or equally what a NNTP client exchanges with an NNTP Server." << std::endl
                  << std::endl;
        return -1;
    }

    std::stringstream s;

    s << argv[1];
    s >> proxy_port;
    s.clear();

    server_addr = argv[2];

    s << argv[3];
    s >> server_port;

    SocketServer in(proxy_port, 5);

    while (1)
    {
        Socket *s = in.Accept();
#ifdef WINDOWS_OS
        unsigned ret;
        _beginthreadex(0, 0, RunProxyThread, (void *)s, 0, &ret);
#else
        pthread_t tid;
        pthread_create(&tid, nullptr, RunProxyThread, (void *)s);
#endif
    }

    return 0;
}
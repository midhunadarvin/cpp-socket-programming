#pragma once

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
#include "impl/TcpConnection.h"

class Proxy
{
public:
    struct ProxyClientThreadContext
    {
        Proxy *self;
        Socket *socket;
    };

    /**
     * Constructor
     */
    Proxy(int proxy_port, std::string server_addr, int server_port) : proxy_port(proxy_port), server_addr(server_addr), server_port(server_port)
    {
        server_socket = new CServerSocket(proxy_port, 5);
        pool = ConnectionPoolFactory< TcpConnection >::create( 4, server_addr, server_port );
    }

    static void *RunProxyClientThreadHelper(void *context)
    {
        ProxyClientThreadContext *proxyClientThreadContext = (ProxyClientThreadContext *)context;
        return proxyClientThreadContext->self->RunProxyClientThread(
            proxyClientThreadContext->socket);
    }

    /**
     * The thread method that will handle exchange of requests between
     * the client and the server
     */
#ifdef WINDOWS_OS
    unsigned __stdcall RunProxyClientThread(void *a);
#else
    void *RunProxyClientThread(void *a);
#endif

    void *StartListeningThread()
    {
        while (1)
        {
            Socket *s = server_socket->Accept();
#ifdef WINDOWS_OS
            unsigned ret;
            _beginthreadex(0, 0, RunProxyThread, (void *)s, 0, &ret);
#else
            pthread_t tid;
            ProxyClientThreadContext proxyClienThreadContext;
            proxyClienThreadContext.self = this;
            proxyClienThreadContext.socket = s;
            pthread_create(&tid, nullptr, &Proxy::RunProxyClientThreadHelper, (void *)&proxyClienThreadContext);
#endif
        }
    }

private:
    int proxy_port;
    std::string server_addr;
    int server_port;
    CServerSocket *server_socket;
    std::unique_ptr< ConnectionPool > pool;
};

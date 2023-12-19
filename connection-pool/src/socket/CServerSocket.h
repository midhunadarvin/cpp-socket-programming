#pragma once
#include "Socket.h"

#define MAX_CONNECTIONS 5
#define SOCKET_BIND_ERROR -1
#define SOCKET_LISTEN_ERROR -1

#ifdef WINDOWS_OS
DWORD WINAPI ListenThreadProc(LPVOID lpParameter);
DWORD WINAPI ClientThreadProc(LPVOID lpParam);
#else
// POSIX
void *ListenThreadProc(void *lpParameter);
void *ClientThreadProc(void *lpParam);
#endif


/**
 * CServerSocket
 * - This class holds the responsibility for maintaining the
 *   proxy server socket.
 */
class CServerSocket : public Socket
{
    int m_ProtocolPort = 3500;
    char Protocol[255];
    int max_connections = MAX_CONNECTIONS;
    struct sockaddr_in socket_address;

public:

    // Constructor
    CServerSocket(int port, int num_of_connections = MAX_CONNECTIONS, TypeSocket type = BlockingSocket);

    Socket* Accept();

    /** Parametrized Thread Routine */
    std::function<void *(void *)> thread_routine;

    bool StartListeningThread(const std::string& node_info, std::function<void *(void *)> pthread_routine);

    // Open the server port and start listening
    bool Open(std::string node_info, std::function<void *(void *)> pthread_routine);

#ifdef WINDOWS_OS
    static DWORD WINAPI ListenThreadProc(LPVOID lpParameter);
    static DWORD WINAPI ClientThreadProc(LPVOID lpParam, ClientTargetPair *pair);
#else
    static void *ListenThreadProc(void *lpParameter);
    static void *ClientThreadProc(void *lpParam);
#endif
};

// typedef void *(*PipelineFunction)(CServerSocket *ptr, void *lptr);

template <typename T>
using PipelineFunction = void *(*)(T *ptr, void *lptr);
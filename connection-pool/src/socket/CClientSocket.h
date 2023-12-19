#pragma once
#include "Socket.h"
#include <string>
// #include "ProtocolHelper.h"

using namespace std;
#define SOCKET int
void Sleep(unsigned int microseconds);

/**
 * CClientSocket
 * - This class holds the responsibility for maintaining the
 *   client socket connection. 
*/
class CClientSocket : public Socket
{
private:
    char m_ServerName[255];
    int m_ServerPort;
    struct sockaddr_in m_Server;
    struct hostent *m_HostPointer;
    unsigned int m_addr;

public:
    // Constructor
    CClientSocket(std::string server_name, int client_port);

    // Resolve the host name or IP address
    bool Resolve(const std::string &host);

    // Connect to the server
    bool Connect();

    // Reconnect to the server
    bool Reconnect();

    ~CClientSocket() { Close(); }
};

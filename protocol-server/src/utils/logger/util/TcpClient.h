#pragma once
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdexcept>
#include <sys/ioctl.h>

const int BUFFER_SIZE = 1024;

class TcpClient
{
public:
    TcpClient(const char *domain, int port)
    {
        std::string host = domain;
        std::string error;
        hostent* m_HostPointer = Resolve(host);

        // Create a socket
        _clientSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (_clientSocket == -1)
        {
            std::cerr << "Error creating socket" << std::endl;
            throw std::runtime_error("Error creating socket");
        }

        // Specify the server address and port
        struct sockaddr_in serverAddress;
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(port);
        serverAddress.sin_addr = *((in_addr *)m_HostPointer->h_addr);
        memset(&(serverAddress.sin_zero), 0, 8);

        inet_pton(AF_INET, domain, &(serverAddress.sin_addr));

        // Connect to the server
        if (connect(_clientSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
        {
            std::cerr << "TcpClient:: Error connecting to the server" << std::endl;
            close(_clientSocket);
            throw std::runtime_error("TcpClient:: Error connecting to the server");
        }
    }

    ~TcpClient() {
        // Close the socket
        close(_clientSocket);
    }

    bool sendTcpMessage(std::string content)
    {
        // Send a message to the server
        ssize_t bytesSent = send(_clientSocket, content.c_str(), content.size(), 0);
        if (bytesSent == -1)
        {
            std::cerr << "Error sending data to the server" << std::endl;
            // Close the socket
            close(_clientSocket);
        }

        return true;
    }

    /**
     * Resolve the host name or IP address
     */
    hostent* Resolve(const std::string &host)
    {
        std::string error;
        hostent *m_HostPointer;
        if (isalpha(host[0]))
        {
            try
            {
                if ((m_HostPointer = gethostbyname(host.c_str())) == 0)
                {
                    std::cout << "Unable to get host endpoint by name " << std::endl;
                    error = strerror(errno);
                    throw std::runtime_error(error);
                }
            }
            catch (std::exception &e)
            {
                std::cout << e.what() << std::endl;
                std::cout << "Unable to get host endpoint by name " << std::endl;
                throw std::runtime_error(error);
            }

            return m_HostPointer;
        }
        else
        {
            try
            {
                /* Convert nnn.nnn address to a usable one */
                unsigned int m_addr = inet_addr(host.c_str());
                m_HostPointer = gethostbyaddr((char *)&m_addr, 4, AF_INET);
            }
            catch (std::exception &e)
            {
                std::cout << e.what() << std::endl;
                std::cout << "Unable to get host endpoint by name " << std::endl;
                throw std::runtime_error(error);
            }
            return m_HostPointer;
        }
    }

private:
    int _clientSocket;
};

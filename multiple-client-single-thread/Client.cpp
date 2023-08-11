/**
 * Client code in cpp for making socket connections
 * to server.
 *
 * Compile :
 * g++ -o Client.exe Client.cpp // compile and link
 *
 * Execute :
 * ./Client.exe 127.0.0.1 8000
 */

#include <stdio.h>  // contains declarations used in most input and output
#include <stdlib.h> // constains declarations for performing general functions Eg. atoi
#include <string.h> // contains helper functions for handling strings
#include <unistd.h> // standard symbolic constants and types ( for read, write, close )

#include <sys/types.h>  // contains definitions of a number of data types used in system calls
#include <sys/socket.h> // contains definitions of structures needed for sockets
#include <netinet/in.h> // contains definitions of structures needed for internet domain addresses.
#include <netdb.h>      // contains definitions for network database operations.

#define INVALID_SOCKET (-1)

/**
 * Helper functions for cross platform compatibility
 */
#ifdef WINDOWS_OS
    void Cleanup() { WSACleanup(); }

    bool StartSocket()
    {
        WORD Ver;
        WSADATA wsd;
        Ver = MAKEWORD(2, 2);
        if (WSAStartup(Ver, &wsd) == SOCKET_ERROR)
        {
            WSACleanup();
            return false;
        }

        return true;
    }

    int SocketGetLastError()
    {
        return WSAGetLastError();
    }

    int CloseSocket(SOCKET s)
    {
        closesocket(s);
        return 0;
    }
#else
    #define SOCKET_ERROR (-1)
    #define SOCKET int
    void Cleanup() {}
    bool StartSocket() { return true; }
    int SocketGetLastError() { return 0xFFFF; }
    int CloseSocket(int s)
    {
        shutdown(s, 2);
        return 0;
    }
#endif

/**
 * Error handler function
 */
void error(const char *msg)
{
    perror(msg); // Interprets the error code and outputs the error description stderr
    exit(1);     // Terminate the program
}

/**
 * Main function
 */
int main(int argc, char *argv[])
{

    int sock_file_descriptor, port_number, read_write_status;
    struct sockaddr_in server_address;
    struct hostent *server;

    char buffer[255];

    if (argc < 3)
    {
        fprintf(stderr, "Useage : %s <hostname> <port-number>\n", argv[0]);
        exit(1);
    }

    // Get the port number from the arguments. atoi converts string to integer
    port_number = atoi(argv[2]);

    // Initialize the socket file descriptor
    // int socket(int domain, int type, int protocol)
    // AF_INET      --> ipv4
    // SOCK_STREAM  --> TCP
    // SOCK_DGRAM   --> UDP
    // protocol     --> default for TCP
    sock_file_descriptor = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (sock_file_descriptor == INVALID_SOCKET)
    {
        error("Error opening socket");
        Cleanup();
        exit(EXIT_FAILURE);
    }

    // Get server
    server = gethostbyname(argv[1]);

    if (server == NULL)
    {
        fprintf(stderr, "Error, No such host");
    }

    // clear / set all bytes to NUL for server_address
    bzero((char *)&server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    // copy all bytes from server to server_address
    bcopy((char *)server->h_addr, (char *)&server_address.sin_addr.s_addr, server->h_length);
    server_address.sin_port = htons(port_number); // host to network short
    memset(&(server_address.sin_zero), 0, 8);

    /**
     * Connect to server
     */
    if (connect(sock_file_descriptor, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        error("Connection to server failed!");
        Cleanup();
        exit(EXIT_FAILURE);
    }

    printf("Connected to the server !\n");

    // receive a message from the client (listen)
    memset(&buffer, 0, sizeof(buffer));
    read_write_status = recv(sock_file_descriptor, buffer, sizeof(buffer), 0);

    if (read_write_status < 0)
    {
        error("Error on reading from server!");
        return -1;
    }
    printf("Server: %s\n", buffer);

    while (1)
    {
        // clear buffer
        memset(&buffer, 0, sizeof(buffer));

        // Get string from std in
        fgets(buffer, 255, stdin);

        printf("Client: %s\n", buffer);

        read_write_status = send(sock_file_descriptor, buffer, strlen(buffer), 0);
   
        if (read_write_status < 0)
        {
            error("Error on writing to server!");
            return -1;
        }

        bool exit_status = strncmp("exit", buffer, 4) == 0;
        if (exit_status)
        {
            break;
        }
    }

    close(sock_file_descriptor);
    return 0;
}
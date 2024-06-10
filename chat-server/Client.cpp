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
#include <map>      // map data structure
#include <string>   // string utilities
#include <iostream> // utilities for input and output
#include <utility>  // pair
#include <thread>   // threads
#include <mutex>    // mutex for threads

#include <sys/types.h>  // contains definitions of a number of data types used in system calls
#include <sys/socket.h> // contains definitions of structures needed for sockets
#include <netinet/in.h> // contains definitions of structures needed for internet domain addresses.
#include <netdb.h>      // contains definitions for network database operations.
#include <sys/time.h>   // time types
#include <sys/select.h> // timeval type and select function
#include <fcntl.h>      // Defines file control options. ( linux )

typedef unsigned long u_long;
#define INVALID_SOCKET (-1)

/**
 * Helper functions for cross platform compatibility
 */
#ifdef WINDOWS_OS
int make_nonblocking(int file_descriptor)
{
    // Set socket to non-blocking
    u_long mode = 1; // 0 for blocking and 1 for non-blocking ; By default socket is blocking
    if (ioctlsocket(sock_file_descriptor, FIONBIO, &mode) != 0)
    {
        printf("ioctlsocket failed\n");
        closesocket(sock_file_descriptor);
        WSACleanup();
        return -1;
    }
    return 0;
}

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

int make_nonblocking(int file_descriptor)
{
    int flags = fcntl(file_descriptor, F_GETFL, 0);
    if (flags == -1)
    {
        printf("fcntl(F_GETFL) failed\n");
        close(file_descriptor);
        return -1;
    }

    flags |= O_NONBLOCK;
    if (fcntl(file_descriptor, F_SETFL, flags) == -1)
    {
        printf("fcntl(F_SETFL) failed\n");
        close(file_descriptor);
        return -1;
    }

    return 0;
}

void Cleanup() {}
bool StartSocket() { return true; }
int SocketGetLastError() { return 0xFFFF; }
int CloseSocket(int s)
{
    shutdown(s, 2);
    return 0;
}
#endif

int sock_file_descriptor;
fd_set readfds, writefds, exceptfds;

// We need a data structure to queue the messages to be written to the socket file descriptor
std::map<int, std::string> client_messages_queue;
std::mutex client_messages_queue_mutex;

/**
 * Error handler function
 */
void error(const char *msg)
{
    perror(msg); // Interprets the error code and outputs the error description stderr
    // exit(1);     // Terminate the program
}

/**
 * Sends a request to the socket file descriptor
 * @param client_socket_file_descriptor
 * @param messag
*/
void sendRequest(int client_socket_file_descriptor, char *message) {
    send(client_socket_file_descriptor, message, 255, 0);
}

/**
 * Listens for inputs and adds it to the message queue 
 * for 
*/
void inputListener() {
    while (true) {
        std::string input;
        std::getline(std::cin, input);

        if (input == "exit") {
            break;
        }

        std::cout << "You entered: " << input << std::endl;
        std::lock_guard<std::mutex> lock(client_messages_queue_mutex);
        client_messages_queue.insert(std::pair<int, std::string>(sock_file_descriptor, input));
    }
}

/**
 * Cleanup the client socket and connection
 */
void closeClientConnection(int client_socket_file_descriptor)
{
    std::string message = "Error reading from socket ! Closing the socket connection for file descriptor : " + std::to_string(client_socket_file_descriptor) + "\n";
    error(message.c_str());
    CloseSocket(client_socket_file_descriptor);
}

/**
 * Process the request function explicitily for server messages
 */
void processRequest(int client_socket_file_descriptor)
{

    if (FD_ISSET(client_socket_file_descriptor, &readfds))
    {
        // Got the new message from the client
        printf("Received new message from the server, file_descriptor: %d\n", client_socket_file_descriptor);
        printf("Processing the server sent message for socket file descriptor :%d\n", client_socket_file_descriptor);

        // Buffer is used to store our messages
        char buffer[255];
        // set the buffer values to NUL i.e clear the buffer
        memset(&buffer, 0, sizeof(buffer));

        // read bytes from socket / receive a message from the server (listen)
        int read_write_status = recv(client_socket_file_descriptor, buffer, sizeof(buffer), 0);

        if (read_write_status == 0)
        {
            return closeClientConnection(client_socket_file_descriptor);
        }
        else if (read_write_status < 0)
        {
            if (errno == EAGAIN)
                return closeClientConnection(client_socket_file_descriptor);
        }

        // Successfully read from socket. Output the stream to stdout
        printf("Server: %s\n", buffer);
    }

    // Check if we have anything to write
    if (FD_ISSET(client_socket_file_descriptor, &writefds))
    {
        std::lock_guard<std::mutex> lock(client_messages_queue_mutex);
        printf("There are messages in queue that are to be written to the file_descriptor: %d\n", client_socket_file_descriptor);
        char *message = (char *)(client_messages_queue[client_socket_file_descriptor]).c_str();
        printf("Message : %s \n", message);
        sendRequest(client_socket_file_descriptor, message);
        client_messages_queue.erase(client_socket_file_descriptor);
    }

    printf("Finished processing the client message for socket file descriptor : %d\n", client_socket_file_descriptor);
}

/**
 * Main function
 */
int main(int argc, char *argv[])
{

    int port_number, read_write_status;
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

    std::thread listenerThread(inputListener);


    /**
     * Set Non Blocking Socket
     */
    make_nonblocking(sock_file_descriptor);

    // The max file descriptor would be the server socket file descriptor, since the new accepted connections will starting from server socket file descriptor + 1
    int max_file_descriptor = sock_file_descriptor;

    // initial timeval param for select function
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;

    while (true)
    {
        // set the file descriptor sets to zero
        FD_ZERO(&readfds);
        FD_ZERO(&writefds);
        FD_ZERO(&exceptfds);

        // Since we need to read from the new socket and also check for exceptions
        FD_SET(sock_file_descriptor, &readfds);
        FD_SET(sock_file_descriptor, &exceptfds);

        // Check if we have any messages to be written
        if (client_messages_queue.find(sock_file_descriptor) != client_messages_queue.end()) {
            // add to the write FD_SET
            FD_SET(sock_file_descriptor, &writefds);
        }

        // keep waiting for new requests and proceed as per request
        int ready = select(max_file_descriptor + 1, &readfds, &writefds, &exceptfds, &tv);
        if (ready > 0)
        {
            // when someone connects or communicates with a message over a dedicated connection
            printf("Data received on port, Processing now... \n");
            // process the request
            processRequest(sock_file_descriptor);
        }
        else if (ready == 0)
        {
            // No connection or no request made
            // none of the socket descriptors are ready
            // printf("Nothing on port : %d\n", port_number);
        }
        else
        {
            // It failed. handle error
            printf("Listening to the server port failed \n");
            Cleanup();
            exit(EXIT_FAILURE);
        }
    }


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

        // receive a message from the client (listen)
        printf("Awaiting server response...\n");

        memset(&buffer, 0, sizeof(buffer));
        read_write_status = recv(sock_file_descriptor, buffer, sizeof(buffer), 0);

        if (read_write_status < 0)
        {
            error("Error on reading from server!");
            return -1;
        }
        printf("Server: %s\n", buffer);

        bool exit_status = strncmp("exit", buffer, 4) == 0;
        if (exit_status)
        {
            break;
        }
    }

    close(sock_file_descriptor);
    listenerThread.join();
    return 0;
}

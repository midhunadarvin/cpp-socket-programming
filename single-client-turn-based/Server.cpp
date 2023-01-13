/**
 * Server code in cpp for creating, listening and 
 * accepting socket connections
 *
 * Compile :
 * g++ -o Server.exe Server.cpp // compile only
 * 
 * Execute :
 * ./Server.exe 8000
 */

#include <stdio.h>       // contains declarations used in most input and output
#include <stdlib.h>      // constains declarations for performing general functions Eg. atoi 
#include <string.h>      // contains helper functions for handling strings
#include <unistd.h>      // standard symbolic constants and types  ( for read, write, close )

#include <sys/types.h>   // contains definitions of a number of data types used in system calls
#include <sys/socket.h>  // contains definitions of structures needed for sockets
#include <netinet/in.h>  // contains definitions of structures needed for internet domain addresses.

#define INVALID_SOCKET (-1)
#define SOCKET_BIND_ERROR (-1)
#define SOCKET_LISTEN_ERROR (-1) 
#define MAX_CONNECTIONS 5

/**
 * Error handler function
*/
void error(const char *msg) {
    perror(msg);   // Interprets the error code and outputs the error description stderr
    exit(1);       // Terminate the program
}

/**
 * Main function
*/
int main(int argc, char *argv[]) {

    if (argc < 2) {
        fprintf(stderr, "Port number not provided. Program terminated\n");
        fprintf(stdout, "Useage : ./Server.exe <port-number>\n");
        exit(1);
    }
    
    int sock_file_descriptor, newsock_file_descriptor, port_number, read_write_status;

    // Buffer is used to store our messages
    char buffer[255];

    //Socket address, internet style.
    struct sockaddr_in server_address, client_address;
    socklen_t client_length;

    // Initialize the socket file descriptor
    // int socket(int domain, int type, int protocol)
    // AF_INET      --> ipv4
    // SOCK_STREAM  --> TCP
    // SOCK_DGRAM   --> UDP
    // protocol = 0 --> default for TCP
    sock_file_descriptor = socket(AF_INET, SOCK_STREAM, 0);

    if (sock_file_descriptor == INVALID_SOCKET) {
        error("Error opening socket");
        return -1;
    }

    // The bzero() function fills the first n bytes of the object pointed to by dst with zero (NUL) bytes.
    // This is done so as to make sure it is clear
    bzero((char*) &server_address, sizeof(server_address));

    // Get the port number from the arguments. atoi converts string to integer
    port_number = atoi(argv[1]);

    server_address.sin_family = AF_INET; // ipv4
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(port_number); // host to network short

    /**
     * Bind the socket to server_address
    */
    if (bind(sock_file_descriptor, (struct sockaddr *) &server_address, sizeof(server_address)) == SOCKET_BIND_ERROR) {
        error("Binding the socket to server failed!");
        return -1;
    }


    /**
     * Listen for connections
    */
    if (listen(sock_file_descriptor, MAX_CONNECTIONS) == SOCKET_LISTEN_ERROR) {
        error("Error while listening for connections!");
        return -1;
    };

    client_length = sizeof(client_address);
 
    newsock_file_descriptor = accept(sock_file_descriptor, (struct sockaddr *) &client_address, &client_length);

    if (newsock_file_descriptor < 0) {
        error("Error on Accept new connection!");
    }

    printf("Accepted new connection!\n");

    while (1) {
        // receive a message from the client (listen)
        printf("Awaiting client response...\n");

        // set the buffer values to NUL i.e clear the buffer
        memset(&buffer, 0, sizeof(buffer));

        // read bytes from socket 
        read_write_status = recv(newsock_file_descriptor, buffer, sizeof(buffer), 0);

        if (read_write_status < 0) {
            error("Error reading from socket !");
        }

        // Successfully read from socket. Output the stream to stdout
        printf("Client : %s\n", buffer);

        // Clear the buffer
        memset(&buffer, 0, sizeof(buffer));

        // Read string from server terminal
        fgets(buffer, 255, stdin);

        // write the buffer data to client socket connection
        read_write_status = send(newsock_file_descriptor, buffer, strlen(buffer), 0);

        if (read_write_status < 0) {
            error("Error writing to socket !");
        }

        bool exit_status = strncmp("exit", buffer, 4) == 0;
        if (exit_status) {
            break;
        }
    }

    close(sock_file_descriptor);
    close(newsock_file_descriptor);
    return 0;
}
/**
 * Server code in cpp for creating, listening and
 * accepting socket connections
 *
 * Compile :
 * g++ -o Server.exe Server_select.cpp // compile and link
 *
 * Execute :
 * ./Server.exe 8000
 */

#include <stdio.h> // contains declarations used in most input and output
#include <stdlib.h> // constains declarations for performing general functions Eg. atoi
#include <string.h> // contains helper functions for handling strings
#include <unistd.h> // standard symbolic constants and types  ( for read, write, close )

#ifdef WINDOWS_OS
#include <windows.h>
#else
#include <arpa/inet.h>  // contains definitions for internet operations
#include <netdb.h>      // contains definitions for network database operations.
#include <netinet/in.h> // contains definitions for the internet protocol family.
#include <pthread.h> // contains function declarations and mappings for threading interfaces and defines a number of constants used by those functions.
#include <sys/socket.h> // contains sockets definitions
#endif
#include <fcntl.h>      // Defines file control options. ( linux )
#include <sys/select.h> // timeval type and select function
#include <sys/time.h>   // time types

typedef unsigned long u_long;
#define INVALID_SOCKET (-1)
#define SOCKET_BIND_ERROR (-1)
#define SOCKET_LISTEN_ERROR (-1)
#define MAX_CONNECTIONS 5

/**
 * Helper functions for cross platform compatibility
 */
#ifdef WINDOWS_OS
#define SOCKET int;
int make_nonblocking(int file_descriptor) {
  // Set socket to non-blocking
  u_long mode = 1; // 0 for blocking and 1 for non-blocking ; By default socket
                   // is blocking
  if (ioctlsocket(sock_file_descriptor, FIONBIO, &mode) != 0) {
    printf("ioctlsocket failed\n");
    closesocket(sock_file_descriptor);
    WSACleanup();
    return -1;
  }
  return 0;
}

void Cleanup() { WSACleanup(); }

bool StartSocket() {
  WORD Ver;
  WSADATA wsd;
  Ver = MAKEWORD(2, 2);
  if (WSAStartup(Ver, &wsd) == SOCKET_ERROR) {
    WSACleanup();
    return false;
  }

  return true;
}

int SocketGetLastError() { return WSAGetLastError(); }

int CloseSocket(SOCKET s) {
  closesocket(s);
  return 0;
}
#else
#define SOCKET_ERROR (-1);
#define SOCKET int;

int make_nonblocking(int file_descriptor) {
  int flags = fcntl(file_descriptor, F_GETFL, 0);
  if (flags == -1) {
    printf("fcntl(F_GETFL) failed\n");
    close(file_descriptor);
    return -1;
  }

  flags |= O_NONBLOCK;
  if (fcntl(file_descriptor, F_SETFL, flags) == -1) {
    printf("fcntl(F_SETFL) failed\n");
    close(file_descriptor);
    return -1;
  }

  return 0;
}

void Cleanup() {}

bool StartSocket() { return true; }

int SocketGetLastError() { return 0xFFFF; }

int CloseSocket(int s) {
  shutdown(s, 2);
  return 0;
}
#endif

// Socket address, internet style.
struct sockaddr_in server_address, client_address;
socklen_t client_length;
fd_set readfds, writefds, exceptfds;
int sock_file_descriptor;

// an array to store the client socket file descriptors
int client_socket_fd_map[5];
int num_of_clients =
    sizeof(client_socket_fd_map) / sizeof(client_socket_fd_map[0]);

/**
 * Error handler function
 */
void error(const char *msg) {
  perror(msg); // Interprets the error code and outputs the error description
               // stderr
  exit(1);     // Terminate the program
}

/**
 * Process the request function explicitily for client messages
 */
void processRequest(int client_socket_file_descriptor) {
  printf("Processing the client message for socket file descriptor :%d\n",
         client_socket_file_descriptor);

  // Buffer is used to store our messages
  char buffer[255];
  // set the buffer values to NUL i.e clear the buffer
  memset(&buffer, 0, sizeof(buffer));

  // read bytes from socket
  int read_write_status =
      recv(client_socket_file_descriptor, buffer, sizeof(buffer), 0);

  if (read_write_status < 0) {
    error("Error reading from socket ! Closing the socket connection...");
    CloseSocket(client_socket_file_descriptor);
    for (int i = 0; i < num_of_clients; i++) {
      if (client_socket_fd_map[i] == client_socket_file_descriptor) {
        client_socket_fd_map[i] = 0;
        char message[] = "Removed client socket connection successfully!";
        break;
      }
    }
  }

  // Successfully read from socket. Output the stream to stdout
  printf("Client : %s\n", buffer);
  printf("Finished processing the client message for socket file descriptor : "
         "%d\n",
         client_socket_file_descriptor);
}
/**
 * Process the request function
 */
void processRequest() {
  // check if we have received any requests in the server listening port
  if (FD_ISSET(sock_file_descriptor, &readfds)) {
    printf("Checking for server socket messages...\n");
    client_length = sizeof(client_address);

#ifdef WINDOWS_OS
    SOCKET client_socket_file_descriptor =
        accept(sock_file_descriptor, (struct sockaddr *)&client_address,
               (int *)&client_length);
#else
    int client_socket_file_descriptor =
        accept(sock_file_descriptor, (struct sockaddr *)&client_address,
               (socklen_t *)&client_length);
#endif

    printf("number of clients : %d\n", num_of_clients);
    int i;
    for (i = 0; i < num_of_clients; i++) {
      if (client_socket_fd_map[i] == 0) {
        printf("Adding to list of sockets at index: %d, file_descriptor: %d\n",
               i, client_socket_file_descriptor);
        client_socket_fd_map[i] = client_socket_file_descriptor;
        char message[] = "Connected Successfully!\n";
        printf("%s", message);
        send(client_socket_file_descriptor, message, 255, 0);

        break;
      }
    }

    if (i == 5) {
      printf("No more space for new connections!");
    }
  }

  printf("Checking for client socket messages...\n");
  // we need to check the existing client connections whether we have any new
  // requests
  for (int i = 0; i < num_of_clients; i++) {
    if (client_socket_fd_map[i] != 0) {
      printf("Found existing client connection at index %d, file_descriptor : "
             "%d\n",
             i, client_socket_fd_map[i]);
      if (FD_ISSET(client_socket_fd_map[i], &readfds)) {
        // Got the new message from the client
        printf("Received new message from the client, file_descriptor: %d\n",
               client_socket_fd_map[i]);
        processRequest(client_socket_fd_map[i]);
      }
    }
  }
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

  int newsock_file_descriptor, port_number, read_write_status;

  // Buffer is used to store our messages
  char buffer[255];

  // Initialize the socket file descriptor
  // int socket(int domain, int type, int protocol)
  // AF_INET      --> ipv4
  // SOCK_STREAM  --> TCP
  // SOCK_DGRAM   --> UDP
  // protocol = 0 --> default for TCP
  sock_file_descriptor = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

  if (sock_file_descriptor == INVALID_SOCKET) {
    error("Error opening socket");
    Cleanup();
    return -1;
  }

  // The bzero() function fills the first n bytes of the object pointed to by
  // dst with zero (NUL) bytes. This is done so as to make sure it is clear
  bzero((char *)&server_address, sizeof(server_address));

  // Get the port number from the arguments. atoi converts string to integer
  port_number = atoi(argv[1]);

  server_address.sin_family = AF_INET; // ipv4
  server_address.sin_addr.s_addr = INADDR_ANY;
  server_address.sin_port = htons(port_number); // host to network short

#ifndef WINDOWS_OS
  /**
   * Set socket options
   */
  int optValue = 0;
  int optLength = sizeof(optValue);
  if (!setsockopt(sock_file_descriptor, SOL_SOCKET, SO_REUSEADDR,
                  (const char *)&optValue, (socklen_t)optLength)) {
    printf("The setsockopt call succeeded!\n");
  } else {
    printf("The setsockopt call failed!\n");
    Cleanup();
    exit(EXIT_FAILURE);
  };
#endif

  /**
   * Bind the socket to server_address
   */
  if (bind(sock_file_descriptor, (struct sockaddr *)&server_address,
           sizeof(server_address)) == SOCKET_BIND_ERROR) {
    error("Binding the socket to server failed!");
    Cleanup();
    return -1;
  }

  /**
   * Listen for connections
   */
  if (listen(sock_file_descriptor, MAX_CONNECTIONS) == SOCKET_LISTEN_ERROR) {
    error("Error while listening for connections!");
    Cleanup();
    exit(EXIT_FAILURE);
  } else {
    printf("Started listening on local port : %d\n", port_number);
  };

  /**
   * Set Non Blocking Socket
   */
  make_nonblocking(sock_file_descriptor);

  // The max file descriptor would be the server socket file descriptor, since
  // the new accepted connections will starting from server socket file
  // descriptor + 1
  int max_file_descriptor = sock_file_descriptor;

  // initial timeval param for select function
  struct timeval tv;
  tv.tv_sec = 1;
  tv.tv_usec = 0;

  while (true) {
    // set the file descriptor sets to zero
    FD_ZERO(&readfds);
    FD_ZERO(&writefds);
    FD_ZERO(&exceptfds);

    // Since we need to read from the new socket and also check for exceptions
    FD_SET(sock_file_descriptor, &readfds);
    FD_SET(sock_file_descriptor, &exceptfds);

    // If we have connected to any clients, we need to make sure to add their
    // file descriptors also to the fd sets
    for (int i = 0; i < num_of_clients; i++) {
      if (client_socket_fd_map[i] != 0) {
        // printf("Adding client socket file descriptor %d to the FD_SETs\n",
        // client_socket_fd_map[i]);
        FD_SET(client_socket_fd_map[i], &readfds);
        FD_SET(client_socket_fd_map[i], &exceptfds);
      }
    }

    // keep waiting for new requests and proceed as per request
    int ready = select(max_file_descriptor + num_of_clients + 1, &readfds,
                       &writefds, &exceptfds, &tv);
    if (ready > 0) {
      // when someone connects or communicates with a message over a dedicated
      // connection
      printf("Data received on port, Processing now... \n");
      // process the request
      processRequest();
    } else if (ready == 0) {
      // No connection or no request made
      // none of the socket descriptors are ready
      // printf("Nothing on port : %d\n", port_number);
    } else {
      // It failed. handle error
      printf("Listening to the server port failed \n");
      Cleanup();
      exit(EXIT_FAILURE);
    }
  }

  /**
   * Close out all socket file descriptors before exit.
   */
  close(sock_file_descriptor);
  for (int i = 0; i < num_of_clients; i++) {
    if (client_socket_fd_map[i] != 0) {
      close(client_socket_fd_map[i]);
      break;
    }
  }
  return 0;
}
// Build : g++ Proxy.cpp -o Proxy.exe -lssl -lcrypto
// Run   : ./Proxy.exe <proxy-port> <server-host> <server-port>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <sstream>    // provides templates and types that enable interoperation between stream buffers and string objects

int proxy_port;
std::string server_address;
int server_port;

#define BUFFER_SIZE 4096

void handle_error(const char *msg) {
    perror(msg);
    ERR_print_errors_fp(stderr);
    exit(1);
}

int main(int argc, char *argv[]) {

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

    server_address = argv[2];

    s << argv[3];
    s >> server_port;

    SSL_CTX *ctx;
    SSL *ssl_client, *ssl_target;
    int server_socket, client_socket, target_socket;
    struct sockaddr_in server_addr, client_addr, target_addr;
    socklen_t client_len = sizeof(client_addr);

    // Initialize SSL library
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    ctx = SSL_CTX_new(SSLv23_client_method());
    if (!ctx) {
        handle_error("SSL_CTX_new");
    }

    // Create server socket
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        handle_error("socket");
    }

    // Configure the server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(proxy_port);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    // Bind the server socket
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        handle_error("bind");
    }

    // Listen for incoming connections
    if (listen(server_socket, 5) < 0) {
        handle_error("listen");
    }

    while (1) {
        // Accept a client connection
        if ((client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len)) < 0) {
            handle_error("accept");
        }

        // Initialize SSL connection for the client
        ssl_client = SSL_new(ctx);
        SSL_set_fd(ssl_client, client_socket);
        if (SSL_accept(ssl_client) < 0) {
            handle_error("SSL_accept");
        }

        // Connect to the target server
        if ((target_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            handle_error("socket");
        }
        memset(&target_addr, 0, sizeof(target_addr));
        target_addr.sin_family = AF_INET;
        target_addr.sin_port = htons(server_port);
        target_addr.sin_addr.s_addr = inet_addr(server_address.c_str());
        if (connect(target_socket, (struct sockaddr *)&target_addr, sizeof(target_addr)) < 0) {
            handle_error("connect");
        }

        // Initialize SSL connection for the target server
        ssl_target = SSL_new(ctx);
        SSL_set_fd(ssl_target, target_socket);
        if (SSL_connect(ssl_target) < 0) {
            handle_error("SSL_connect");
        }

        // Handle data transfer between client and target using SSL
        char buffer[BUFFER_SIZE];
        int bytes;

        while ((bytes = SSL_read(ssl_client, buffer, sizeof(buffer)) > 0)) {
            if (SSL_write(ssl_target, buffer, bytes) < 0) {
                handle_error("SSL_write to target");
            }
        }

        while ((bytes = SSL_read(ssl_target, buffer, sizeof(buffer)) > 0)) {
            if (SSL_write(ssl_client, buffer, bytes) < 0) {
                handle_error("SSL_write to client");
            }
        }

        // Close the SSL connections and sockets
        SSL_shutdown(ssl_client);
        SSL_free(ssl_client);
        SSL_shutdown(ssl_target);
        SSL_free(ssl_target);
        close(target_socket);
        close(client_socket);
    }

    // Clean up SSL
    SSL_CTX_free(ctx);

    // Close the server socket
    close(server_socket);

    return 0;
}

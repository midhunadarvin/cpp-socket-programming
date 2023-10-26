/**
 * Build : g++ Server.cpp -o Server.exe libuv.a -Iinclude
 * Run   : ./Server.exe
*/
#include <uv.h>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>

using namespace std;

// Define the target IP address and port

int proxy_port;
std::string server_host;
int server_port;

// Structure to represent client and target connections
typedef struct
{
    uv_tcp_t client;
    uv_tcp_t target;
} ClientTargetPair;

// Initialize libuv loop
uv_loop_t *loop;

// Callback function for memory allocation when reading data
void on_alloc(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf)
{
    cout << "on_alloc" << endl;
    // Allocate a buffer for incoming data
    buf->base = (char *)malloc(suggested_size);
    buf->len = suggested_size;
}

// Callback function for when data is received
void on_read(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf)
{
    cout << "on_read : " << nread << endl;
    if (nread < 0)
    {
        // Error or end of stream, close both client and target connections
        uv_close((uv_handle_t *)stream, NULL);
        uv_close((uv_handle_t *)stream->data, NULL);
        free(buf->base);
        return;
    }

    if (nread > 0)
    {
        // Forward data from source to target
        uv_write_t *write_req = new uv_write_t;
        uv_buf_t write_buf = uv_buf_init(buf->base, nread);
        uv_write(write_req, (uv_stream_t *)stream->data, &write_buf, 1, nullptr);
        free(buf->base);
    }
}

// Callback function for when a new client connection is accepted
void on_new_connection(uv_stream_t *server, int status)
{
    cout << "on_new_connection" << endl;
    if (status < 0)
    {
        fprintf(stderr, "New connection error: %s\n", uv_strerror(status));
        return;
    }

    // Create a client-target pair
    ClientTargetPair *pair = (ClientTargetPair *)malloc(sizeof(ClientTargetPair));

    // Initialize client and target sockets
    uv_tcp_init(loop, &pair->client);
    uv_tcp_init(loop, &pair->target);

    // Connect to target database
    std::string error;
    hostent *he;
    try
    {
        if ((he = gethostbyname(server_host.c_str())) == 0)
        {
            std::cout << "Unable to get host endpoint by name " << std::endl;
            error = strerror(errno);
            throw error;
        }
    }
    catch (std::exception &e)
    {
        std::cout << e.what() << std::endl;
        std::cout << "Unable to get host endpoint by name " << std::endl;
    }
    char * ip_address = inet_ntoa(*(struct in_addr *)he->h_addr);
    // Step 4
    std::cout << "IP address of " << he->h_name << " is: " << ip_address << std::endl;
    std::cout << ip_address << ":" << server_port << std::endl;
    // Accept the incoming client connection
    if (uv_accept(server, (uv_stream_t *)&pair->client) == 0)
    {
        // Connect to the target server
        uv_connect_t connect_req;
        struct sockaddr_in client_addr;
        uv_ip4_addr(ip_address, server_port, &client_addr);
        uv_tcp_connect(&connect_req, &pair->target, (const struct sockaddr *)&client_addr, NULL);

        // Set up data forwarding between client and target
        pair->client.data = &pair->target;
        pair->target.data = &pair->client;

        uv_read_start((uv_stream_t *)&pair->client, on_alloc, on_read);
        uv_read_start((uv_stream_t *)&pair->target, on_alloc, on_read);
    }
    else
    {
        uv_close((uv_handle_t *)&pair->client, NULL);
        uv_close((uv_handle_t *)&pair->target, NULL);
    }
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

    server_host = argv[2];

    s << argv[3];
    s >> server_port;
    // Initialize libuv loop
    loop = uv_default_loop();

    // Create a TCP server for the proxy
    uv_tcp_t server;
    uv_tcp_init(loop, &server);

    struct sockaddr_in bind_addr;
    uv_ip4_addr("0.0.0.0", proxy_port, &bind_addr);

    // Bind and listen on a port
    uv_tcp_bind(&server, (const struct sockaddr *)&bind_addr, 0);
    int listen_result = uv_listen((uv_stream_t *)&server, 128, on_new_connection);

    if (listen_result)
    {
        fprintf(stderr, "Listen error: %s\n", uv_strerror(listen_result));
        return 1;
    }

    printf("Proxy server listening on port %d\n", proxy_port);

    // Run the libuv event loop
    uv_run(loop, UV_RUN_DEFAULT);

    return 0;
}

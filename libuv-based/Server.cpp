
#include "include/uv.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

using namespace std;

// Define the target IP address and port
#define TARGET_IP "127.0.0.1"
#define TARGET_PORT 8123

// Structure to represent client and target connections
typedef struct {
    uv_tcp_t client;
    uv_tcp_t target;
} ClientTargetPair;

// Initialize libuv loop
uv_loop_t *loop;

// Callback function for memory allocation when reading data
void on_alloc(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) {
    cout << "on_alloc" << endl;
    // Allocate a buffer for incoming data
    buf->base = (char *)malloc(suggested_size);
    buf->len = suggested_size;
}

// Callback function for when data is received
void on_read(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf) {
    cout << "on_read" << endl;
    if (nread < 0) {
        // Error or end of stream, close both client and target connections
        uv_close((uv_handle_t *)stream, NULL);
        uv_close((uv_handle_t *)stream->data, NULL);
        free(buf->base);
        return;
    }

    if (nread > 0) {
        // Forward data from source to target
        uv_write_t write_req;
        uv_buf_t write_buf = uv_buf_init(buf->base, nread);
        uv_write(&write_req, (uv_stream_t *)stream->data, &write_buf, 1, NULL);
        free(buf->base);
    }
}

// Callback function for when a new client connection is accepted
void on_new_connection(uv_stream_t *server, int status) {
    cout << "on_new_connection" << endl;
    if (status < 0) {
        fprintf(stderr, "New connection error: %s\n", uv_strerror(status));
        return;
    }

    // Create a client-target pair
    ClientTargetPair *pair = (ClientTargetPair *) malloc(sizeof(ClientTargetPair));

    // Initialize client and target sockets
    uv_tcp_init(loop, &pair->client);
    uv_tcp_init(loop, &pair->target);

    // Accept the incoming client connection
    if (uv_accept(server, (uv_stream_t *)&pair->client) == 0) {
        // Connect to the target server
        uv_connect_t connect_req;
        struct sockaddr_in client_addr;
        uv_ip4_addr(TARGET_IP, TARGET_PORT, &client_addr);
        uv_tcp_connect(&connect_req, &pair->target, (const struct sockaddr *)&client_addr, NULL);

        // Set up data forwarding between client and target
        pair->client.data = &pair->target;
        pair->target.data = &pair->client;

        uv_read_start((uv_stream_t *)&pair->client, on_alloc, on_read);
        uv_read_start((uv_stream_t *)&pair->target, on_alloc, on_read);
    } else {
        uv_close((uv_handle_t *)&pair->client, NULL);
        uv_close((uv_handle_t *)&pair->target, NULL);
    }
}

int main() {
    // Initialize libuv loop
    loop = uv_default_loop();

    // Create a TCP server for the proxy
    uv_tcp_t server;
    uv_tcp_init(loop, &server);

    struct sockaddr_in bind_addr;
    uv_ip4_addr("0.0.0.0", 8081, &bind_addr);

    // Bind and listen on a port
    uv_tcp_bind(&server, (const struct sockaddr *)&bind_addr, 0);
    int listen_result = uv_listen((uv_stream_t *)&server, 128, on_new_connection);

    if (listen_result) {
        fprintf(stderr, "Listen error: %s\n", uv_strerror(listen_result));
        return 1;
    }

    printf("Proxy server listening on port 8081\n");

    // Run the libuv event loop
    uv_run(loop, UV_RUN_DEFAULT);

    return 0;
}

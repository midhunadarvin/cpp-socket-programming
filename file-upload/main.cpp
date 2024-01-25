/**
 * g++ main.cpp -o main.exe
 */
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <thread>
#include <mutex>
#include <vector>
#include <cstdint>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

// Constants
const int PORT = 8080;
const int BUFFER_SIZE = 8192;

// Mutex for thread safety
std::mutex coutMutex;

// Function to handle client requests
void handleClient(int clientSocket) {
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));

    // Receive HTTP request
    ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead <= 0) {
        perror("Error receiving data");
        close(clientSocket);
        return;
    }

    // Parse the HTTP request
    std::istringstream request(buffer);
    std::string method, path, http_version;
    request >> method >> path >> http_version;

    // Handle only POST requests
    if (method == "POST") {
        // Find Content-Length header to determine the size of the file
        size_t contentLength = 0;
        std::string line;
        while (std::getline(request, line) && line != "\r\n") {
            std::cout << line << std::endl;
            if (line.substr(0, 16) == "Content-Length: ") {
                contentLength = std::stoul(line.substr(16));
            }
        }
        std::cout << "contentLength : " << contentLength << std::endl;
        // Receive file data
        std::vector<char> fileData(contentLength, 0);
        bytesRead = recv(clientSocket, fileData.data(), contentLength, 0);

        if (bytesRead <= 0) {
            perror("Error receiving file data");
            close(clientSocket);
            return;
        }

        // Save the received file
        {
            std::lock_guard<std::mutex> lock(coutMutex);
            std::ofstream outputFile("uploaded_file.jpeg", std::ios::out | std::ios::binary);
            outputFile.write(fileData.data(), contentLength);
            std::cout << "File uploaded successfully.\n";
        }
    }

    // Send a simple response
    const char* response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Connection: close\r\n\r\n"
        "File uploaded successfully.\r\n";

    send(clientSocket, response, strlen(response), 0);

    close(clientSocket);
}

int main() {
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientLen = sizeof(clientAddr);

    // Create socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        perror("Error creating socket");
        return 1;
    }

    // Set up server address structure
    memset((char*)&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    // Bind the socket
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Error binding socket");
        close(serverSocket);
        return 1;
    }

    // Listen for connections
    listen(serverSocket, 5);

    std::cout << "Server listening on port " << PORT << "...\n";

    while (true) {
        // Accept a connection
        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientLen);
        if (clientSocket < 0) {
            perror("Error accepting connection");
            continue;
        }

        // Handle the client request in a separate thread
        std::thread(handleClient, clientSocket).detach();
    }

    // Close the server socket
    close(serverSocket);

    return 0;
}

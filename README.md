# Socket Programming in CPP

### 1. Socket Chat app

**Start Server**
    
    g++ -o Server.exe Server.cpp 
    ./Server.exe 8000
    
**Start Client**

    g++ -o Client.exe Client.cpp
    ./Client.exe 127.0.0.1 8000
    

![socket-programming](https://user-images.githubusercontent.com/5779375/212344107-b81c0a09-bc3e-42d0-b0f1-58aec8637875.gif)

### Single Client Implementation

- This implementation supports only a single clients connected to the server
- The client can send message to the server, and the server can send back a message in a turn-based fashion.

### Multiple Client Implementation

- This implementation supports multiple clients connected to the server
- The clients can only send message to the server, the server cannot send back any message as of now

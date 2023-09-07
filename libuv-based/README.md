# Libuv based proxy implementation
The traditional accept method for handling incoming client connections in a server has some drawbacks, and the event-driven approach can offer several advantages:

Drawbacks of the accept Method:

- Blocking: The accept function is typically used in a blocking manner. When a new client connection arrives, the server thread or process calling accept blocks until a connection is accepted. This can limit the server's ability to handle multiple clients concurrently without creating a new thread or process for each connection.

- Scalability: Creating a new thread or process for each accepted connection can lead to scalability issues. Managing a large number of threads or processes can be resource-intensive and may not be practical on systems with limited resources.

- Complexity: Managing multiple threads or processes for handling connections can introduce complex synchronization and coordination challenges. Dealing with thread safety and race conditions can make the code more error-prone and harder to maintain.

Advantages of the Event-Driven Approach:

- Asynchronous: The event-driven approach is asynchronous, meaning that it doesn't block when accepting connections or handling data. Instead, it uses a single event loop to efficiently manage multiple connections concurrently without the need for multiple threads or processes.

- Efficiency: It's more efficient in terms of resource usage because it doesn't require the overhead of creating and managing a separate thread or process for each connection. Event-driven servers can handle a large number of concurrent connections with relatively low resource consumption.

- Simplicity: The event-driven approach can lead to simpler and more straightforward code. You don't need to deal with the complexities of multithreading, and the event-driven model naturally separates the concerns of connection acceptance and data handling.

- Scalability: Event-driven servers are inherently scalable. They can efficiently handle a large number of connections without suffering from the thread or process management issues that can plague the accept method.

- Flexibility: Event-driven architectures are often highly adaptable and can be used for various server types, including web servers, proxies, chat servers, and more.

### Build

```
g++ Server.cpp -o Server.exe libuv.a -Iinclude
```

Please note that the libuv.a has been build for MacOS Arm64 processor. If you want to build it for your machine specifications, follow these steps.

```
git clone https://github.com/libuv/libuv.git
cd libuv
sh autogen.sh
./configure
make
```

The libuv.a will be build in libuv/.libs folder. Copy this file and the libuv/include folder to your working directory. 
More details : https://docs.libuv.org/en/v1.x/guide/introduction.html#code

### Run

```
./Server.exe
```
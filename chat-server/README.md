### Chat Server

In this version we would be communicating from client to client via a chat server

The chat server serves as the intermediatory to facilitate communication between 2 clients.

For simplicity, we will connect the second client that connects to the server to the first client.

In real world, we would need to maintain the connection details via their unique ids or ips.

#### Step 1 :

Build and start the server

```
Compile :
g++ -o Server.exe Server.cpp // compile and link

Execute :
./Server.exe 8000
```


#### Step 2 :

Build the client code

```
Compile :
g++ -o Client.exe Client.cpp
```

#### Step 3 :

Run the following in two separate terminals to run two clients :

```
./Client.exe 127.0.0.1 8000
```

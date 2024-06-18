# Simple Protocol Client

This project can be used to quickly prototype a client application to a Protocol Server. 

### Build

```
make
```

### Run

Go to the protocol-server folder, compile and run the protocol-server. The protocol server should be running on port 8080. 
Ensure you are using the `SyncApiProtocolHandler` on the server to handle the payload synchronously.

Run the protocol client :
```
./bin/protocol_client
```

### Test

You should get the response as shown below :
```
[src/main.cpp line:12] INFO Hello world!
[src/main.cpp line:28] INFO Resolved (Target) Host: localhost
[src/main.cpp line:29] INFO Resolved (Target) Port: 8080
IP address of localhost is: 127.0.0.1[src/main.cpp line:44] INFO HTTP/1.1 200 OK
```

### Clean

```
make clean
```
# Simple Protocol Client

This project can be used to quickly prototype a client application to a Protocol Server. 

### Build

```
make
```

### Run

Go to the protocol-server folder, compile and run the protocol-server. The protocol server should be running on port 8080. 
Ensure you are using the `SyncApiProtocolHandler` on the server.

Run the protocol client :
```
./bin/protocol_client
```

### Protocol

The client sends a `CONNECT_REQUEST` packet to the server 

```
struct CONNECT_PACKET {
	int packet_type;
	char action[255];
};
```

The server returns a `CONNECT_REQUEST_RESPONSE`, which indicates an async task has been scheduled. It returns the `task_id` which can be used 
to check the status of the task.

```
struct CONNECT_RESPONSE_PACKET {
	int packet_type;
	char flag; // 0 -> no, 1 -> yes
	char task_id[255];
};
```

The client sends a `CHECK_STATUS_PACKET` to check the status of the task

```
struct CHECK_STATUS_PACKET {
	int packet_type;
	char task_id[255];
};
```

The server returns a `CHECK_STATUS_RESPONSE_PACKET` 

```
struct CHECK_STATUS_RESPONSE_PACKET {
	int packet_type;
	char status[255]; // pending , complete
	char response[255];
};
```

### Test

You should get the response as shown below :
```
[src/main.cpp line:9] INFO Hello world!
[src/main.cpp line:25] INFO Resolved (Target) Host: localhost
[src/main.cpp line:26] INFO Resolved (Target) Port: 8080
[src/main.cpp line:48] INFO Checking for status ...
[src/main.cpp line:63] INFO pending
[src/main.cpp line:48] INFO Checking for status ...
[src/main.cpp line:63] INFO pending
[src/main.cpp line:48] INFO Checking for status ...
[src/main.cpp line:63] INFO pending
[src/main.cpp line:48] INFO Checking for status ...
[src/main.cpp line:63] INFO pending
[src/main.cpp line:48] INFO Checking for status ...
[src/main.cpp line:63] INFO completed
```

### Clean

```
make clean
```
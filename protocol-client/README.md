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

```
curl http://localhost:8080 -v
```

You should get the response as shown below :
```
* Connected to localhost (127.0.0.1) port 8080
> GET / HTTP/1.1
> Host: localhost:8080
> User-Agent: curl/8.7.1
> Accept: */*
>
* Request completely sent off
< HTTP/1.1 200 OK
* Connection #0 to host localhost left intact
```

### Clean

```
make clean
```
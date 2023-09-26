# Passthrough Proxy

This implementation is based on c++ sockets API and kernel level multithreading using `select`

### Build 

```
g++ Proxy.cpp Socket.cpp -o Proxy.exe
```

### Run

```
./Proxy.exe <proxy-port> <target-host> <target-port>
```

### Docker containerization

```
docker compose up -d 
```

The proxy will be accepting connections at 8080 and it will connect to the clickhouse instance in the docker compose.

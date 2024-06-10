# Simple Protocol Server

This project can be used to quickly prototype a protocol server. 

The flow of requests can be represented as shown below :
```
socket port <=> Pipeline <=> Handler
```

The author is required to make changes in the `pipeline` folder and the `handler` folder.

### Build

```
make
```

### Run
```
./bin/my_program
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
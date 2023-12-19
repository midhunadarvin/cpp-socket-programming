cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(UNITTEST True)

project(ConnectionPool VERSION 1.0)

include_directories(include)
add_executable(
    ConnectionPool
        src/main.cpp
        test/tests.cpp
        src/connection.cpp
        src/pool.cpp
        src/proxy.cpp

        src/socket/ProtocolHelper.cpp
        src/socket/Socket.cpp
        src/socket/CClientSocket.cpp
        src/socket/CServerSocket.cpp
        src/socket/SocketSelect.cpp
)

target_compile_definitions(ConnectionPool PRIVATE UNITTEST=True)
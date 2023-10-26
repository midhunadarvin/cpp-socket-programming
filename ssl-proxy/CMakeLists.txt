cmake_minimum_required(VERSION 3.12)
project(SslProxy)

# Add your source files here (e.g., main.cpp)
add_executable(Proxy Proxy.cpp)

# Find the OpenSSL package
find_package(OpenSSL REQUIRED)

if(OPENSSL_FOUND)
    # Link the 'ssl' and 'crypto' libraries to your executable
    target_link_libraries(Proxy OpenSSL::SSL OpenSSL::Crypto)
else()
    message(FATAL_ERROR "OpenSSL not found. Please install OpenSSL.")
endif()
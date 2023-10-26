FROM arm64v8/ubuntu:latest AS builder

RUN apt-get update \
  && apt-get upgrade \
  && apt-get install -y \
    build-essential \
    libtool \
    automake \
    dpkg \
    cmake \
    libuv1-dev \
    ccache \
    python3 \
    bash

RUN apt-get install -y \
    intltool \
    autoconf \
    m4

# Set the working directory inside the container
WORKDIR /app

# Copy the CMake project files into the container
COPY . /app

# RUN cd libuv && rm -rf build && rm -rf .libs
# RUN cd libuv && sh autogen.sh && ./configure && make

# Build the dependencies
RUN g++ Server.cpp -o Server.exe -luv

CMD ["/app/Server.exe", "8080", "database", "8123"]

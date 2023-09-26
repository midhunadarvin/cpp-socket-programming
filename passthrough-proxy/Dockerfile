FROM arm64v8/alpine:latest AS builder

RUN apk update \
  && apk upgrade \
  && apk add --no-cache \
    clang \
    clang-dev \
    alpine-sdk \
    dpkg \
    cmake \
    ccache \
    python3 \
    bash

RUN ln -sf /usr/bin/clang /usr/bin/cc \
  && ln -sf /usr/bin/clang++ /usr/bin/c++ \
  && update-alternatives --install /usr/bin/cc cc /usr/bin/clang 10\
  && update-alternatives --install /usr/bin/c++ c++ /usr/bin/clang++ 10\
  && update-alternatives --auto cc \
  && update-alternatives --auto c++ \
  && update-alternatives --display cc \
  && update-alternatives --display c++ \
  && ls -l /usr/bin/cc /usr/bin/c++ \
  && cc --version \
  && c++ --version

# Set the working directory inside the container
WORKDIR /app

# Copy the CMake project files into the container
COPY . /app

# Build the dependencies
RUN g++ Proxy.cpp Socket.cpp -o Proxy.exe 

FROM arm64v8/alpine:latest AS runtime

RUN apk update \
  && apk upgrade \
  && apk add --no-cache \
    clang \
    clang-dev \
    alpine-sdk \
    dpkg \
    cmake \
    ccache \
    python3 \
    bash

RUN ln -sf /usr/bin/clang /usr/bin/cc \
  && ln -sf /usr/bin/clang++ /usr/bin/c++ \
  && update-alternatives --install /usr/bin/cc cc /usr/bin/clang 10\
  && update-alternatives --install /usr/bin/c++ c++ /usr/bin/clang++ 10\
  && update-alternatives --auto cc \
  && update-alternatives --auto c++ \
  && update-alternatives --display cc \
  && update-alternatives --display c++ \
  && ls -l /usr/bin/cc /usr/bin/c++ \
  && cc --version \
  && c++ --version

COPY --from=0 /app/Proxy.exe /bin/Proxy.exe

CMD ["/bin/Proxy.exe", "8080", "database", "8123"]
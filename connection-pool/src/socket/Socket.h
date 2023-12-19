/*
   Socket.h

   Copyright (C) 2002-2017 René Nyffenegger

   This source code is provided 'as-is', without any express or implied
   warranty. In no event will the author be held liable for any damages
   arising from the use of this software.

   Permission is granted to anyone to use this software for any purpose,
   including commercial applications, and to alter it and redistribute it
   freely, subject to the following restrictions:

   1. The origin of this source code must not be misrepresented; you must not
      claim that you wrote the original source code. If you use this source code
      in a product, an acknowledgment in the product documentation would be
      appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not be
      misrepresented as being the original source code.

   3. This notice may not be removed or altered from any source distribution.

   René Nyffenegger rene.nyffenegger@adp-gmbh.ch
*/

#pragma once

#define SOCKET int
#define INVALID_SOCKET (-1)

#if WINDOWS_OS
#include <WinSock2.h>
#else

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#endif

#include <sys/select.h>
#include <string>
#include <iostream>

enum TypeSocket {
    BlockingSocket,
    NonBlockingSocket
};

int make_nonblocking(int socket_file_descriptor);

class Socket {
public:
    virtual ~Socket();

    Socket(const Socket &);

    Socket &operator=(Socket const &);

    void CreateSocket();

    virtual bool isConnected();

    virtual std::string ReceiveLine();

    virtual std::string ReceiveBytes();

    // The parameter of SendLine is not a const reference
    // because SendLine modifies the std::string passed.
    virtual void SendLine(std::string);

    // The parameter of SendBytes is a const reference
    // because SendBytes does not modify the std::string passed
    // (in contrast to SendLine).
    virtual void SendBytes(char *s, int length);

    int GetSocket();

    virtual void Close();

protected:
    friend class SocketSelect;

    friend class CServerSocket;

    friend class CClientSocket;

    friend class CClientSSLSocket;

    Socket(SOCKET s);

    Socket();

    SOCKET s_;

    int *refCounter_;

private:
    static void Start();

    static void End();

    static int nofSockets_;
};

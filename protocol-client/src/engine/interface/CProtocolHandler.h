#ifndef PROTOCOL_HANDLER_H
#define PROTOCOL_HANDLER_H

#include "../socket/CServerSocket.h"
#include "./BaseHandler.h"
#include "../constants.h"

class CProtocolHandler : BaseHandler
{
public:
    CProtocolHandler() {}

    virtual std::string HandleData(std::string buffer, unsigned long buffer_length, EXECUTION_CONTEXT *exec_context) = 0;

    virtual ~CProtocolHandler() {}
};

#endif
#pragma once

#include "../engine/interface/CProtocolHandler.h"
class DefaultProtocolHandler : public CProtocolHandler {
    std::string HandleData(std::string buffer, unsigned long buffer_length, EXECUTION_CONTEXT *exec_context) override;
};

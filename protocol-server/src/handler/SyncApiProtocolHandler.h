#pragma once

#include "../engine/interface/CProtocolHandler.h"
#include "../utils/scheduler/SimpleScheduler.h"

class SyncApiProtocolHandler : public CProtocolHandler {
public:
    std::string HandleData(std::string buffer, unsigned long buffer_length, EXECUTION_CONTEXT *exec_context) override;
};

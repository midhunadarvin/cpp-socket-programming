#pragma once

#include "../engine/interface/CProtocolHandler.h"
#include "../utils/scheduler/SimpleScheduler.h"

class SyncApiProtocolHandler : public CProtocolHandler {
public:
    std::map<std::string, std::string> response_map;
    std::string HandleData(std::string buffer, unsigned long buffer_length, EXECUTION_CONTEXT *exec_context) override;
};

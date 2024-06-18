#pragma once
#include "strategy/LogStrategy.h"
#include "strategy/DbLogStrategy.h"
#include "strategy/FileLogStrategy.h"
#include "strategy/NetLogStrategy.h"
#include "strategy/NullLogStrategy.h"

class LoggerFactory
{
public:
    static LogStrategy *CreateLogger(const std::string& loggertype)
    {
        if (loggertype == "DB")
            return (LogStrategy *) new DbLogStrategy();
        else if (loggertype == "FILE")
            return (LogStrategy *) new FileLogStrategy();
        else if (loggertype == "NET")
            return (LogStrategy *) new NetLogStrategy();
        else
            return (LogStrategy *) new NullLogStrategy();
    }
};

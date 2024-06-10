#pragma once
#include "LogStrategy.h"
#include "../content-writer/BaseContentWriter.h"
#include "../content-writer/DbContentWriter.h"

class DbLogStrategy : public LogStrategy
{
public:
    DbLogStrategy(): LogStrategy()
    {
        this->wt = new DbContentWriter();
    }
    bool DoLog(std::string logitem) override
    {
        return wt->Write(logitem);
    }
};

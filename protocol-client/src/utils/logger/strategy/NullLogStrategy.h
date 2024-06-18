#pragma once
#include "LogStrategy.h"
#include "../content-writer/ConsoleContentWriter.h"
#include <iostream>

class NullLogStrategy: public LogStrategy {
public:
    NullLogStrategy(): LogStrategy()
    {
        this->wt = new ConsoleContentWriter();
    }
    bool DoLog(std::string logItem) override {
       // Log into the Console
       return this->wt->WriteToMedia(logItem);
    } 
};

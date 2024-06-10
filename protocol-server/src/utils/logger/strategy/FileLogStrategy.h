#pragma once
#include "LogStrategy.h"
#include <ctime>
#include "../content-writer/BaseContentWriter.h"
#include "../content-writer/FileContentWriter.h"

class FileLogStrategy : public LogStrategy
{
private:
    std::string fileName;
public:
    FileLogStrategy()
    {
        initFileContentWriter();
    }

    static const std::string getCurrentDate() {
        char buffer[80];
        time_t now = time(nullptr);
        struct tm time_struct = *localtime(&now);
        // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
        // for more information about date/time format
        strftime(buffer, sizeof(buffer), "%Y-%m-%d", &time_struct);
        return buffer;
    }

    void initFileContentWriter() {
        fileName = getCurrentDate();
        std::string logFileName = "asabru-log-" + fileName + ".txt";
        std::cout << logFileName << std::endl;
        this->wt = new FileContentWriter(logFileName);
    }

    bool DoLog(std::string logItem) override
    {
        if (fileName != getCurrentDate())
            initFileContentWriter();
        return this->wt->Write(logItem);
    }
};

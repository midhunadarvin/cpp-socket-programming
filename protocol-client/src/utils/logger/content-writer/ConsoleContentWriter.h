#pragma once
#include "BaseContentWriter.h"
#include "../util/SQLAccess.h"

class ConsoleContentWriter : public BaseContentWriter
{
public:

    bool WriteToMedia(const std::string logcontent) override
    {
        std::cout << logcontent << std::endl;
        return true;
    }

private:
    std::string _con_str;
    SQLAccess *access;
};
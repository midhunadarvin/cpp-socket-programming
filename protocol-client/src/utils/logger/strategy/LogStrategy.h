#pragma once
#include <string>
#include "../content-writer/BaseContentWriter.h"
class LogStrategy
{
protected:
    BaseContentWriter *wt;
public:
    // DoLog is our Template method
    // Concrete classes will override this
    virtual bool DoLog(std::string logItem) = 0;
    virtual bool Log(std::string app, std::string key, std::string cause)
    {
        return DoLog(app + " " + key + " " + cause);
    }

    bool Empty() {
        return wt->Empty();
    }

    bool Flush() {
        return wt->Flush();
    }
};

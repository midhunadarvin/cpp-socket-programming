#pragma once
#include "BaseContentWriter.h"
#include "../util/SQLAccess.h"

class DbContentWriter : public BaseContentWriter
{
public:
    DbContentWriter() : _con_str("./Logstorage.db")
    {
        access = new SQLAccess(_con_str);
        if (access->Open())
        {
            std::string query = "CREATE TABLE IF NOT EXISTS logs (logcontent TEXT);";
            access->executeQuery(query);
            access->Close();
        }
    }

    bool WriteToMedia(const std::string logcontent) override
    {

        if (access->Open())
        {
            std::string query = "INSERT INTO logs VALUES('" + logcontent + "');";
            bool result = access->executeQuery(query);
            access->Close();
            return result;
        }

        return false;
    }

private:
    std::string _con_str;
    SQLAccess *access;
};
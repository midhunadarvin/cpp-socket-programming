#pragma once
#include <string>
#include <sqlite3.h>
#include <iostream>
#include <string>
#include <vector>

class SQLAccess {
public:
    SQLAccess(const std::string& dbName) : databaseName(dbName), db(nullptr) {
        if (sqlite3_open(databaseName.c_str(), &db) != SQLITE_OK) {
            std::cerr << "Error opening database: " << sqlite3_errmsg(db) << std::endl;
        }
    }

    ~SQLAccess() {
        if (db) {
            sqlite3_close(db);
        }
    }

    bool Open() {
        return db != nullptr;
    }

    void Close() {
        // You may perform additional cleanup if needed.
    }

    bool executeQuery(const std::string& query) {
        char* errorMessage = nullptr;
        int result = sqlite3_exec(db, query.c_str(), nullptr, nullptr, &errorMessage);

        if (result != SQLITE_OK) {
            std::cerr << "Error executing query: " << errorMessage << std::endl;
            sqlite3_free(errorMessage);
            return false;
        }

        return true;
    }

    std::vector<std::vector<std::string>> executeSelect(const std::string& query) {
        std::vector<std::vector<std::string>> resultRows;

        sqlite3_stmt* statement;
        if (sqlite3_prepare_v2(db, query.c_str(), -1, &statement, nullptr) == SQLITE_OK) {
            int columnCount = sqlite3_column_count(statement);

            while (sqlite3_step(statement) == SQLITE_ROW) {
                std::vector<std::string> row;
                for (int i = 0; i < columnCount; ++i) {
                    const char* columnValue = reinterpret_cast<const char*>(sqlite3_column_text(statement, i));
                    row.push_back(columnValue ? columnValue : "");
                }
                resultRows.push_back(row);
            }

            sqlite3_finalize(statement);
        } else {
            std::cerr << "Error preparing statement: " << sqlite3_errmsg(db) << std::endl;
        }

        return resultRows;
    }

private:
    std::string databaseName;
    sqlite3* db;
};

#ifndef SOSC_DATABASE_H
#define SOSC_DATABASE_H

#include "sqlite/sqlite3.h"
#include "../utils/time.hpp"
#include <vector>
#include <string>

#define DB_COL_TEXT 1
#define DB_COL_BLOB 2

namespace sosc {
namespace db {
class Query;

class ResultSet {
public:
    bool IsOpen() const;
    bool IsReadable() const;
    bool Step();

    template<typename T>
    T Get(int column);
    std::string Get(int column, int type = DB_COL_TEXT);
private:
    ResultSet(Query* query, sqlite3_stmt* stmt);
    sqlite3_stmt* statement;
    Query* query;
    bool readable;

    friend class Query;
};

double ResultSet::Get<double>(int column);
int32_t ResultSet::Get<int32_t>(int column);
int64_t ResultSet::Get<int64_t>(int column);
sosc::time ResultSet::Get<sosc::time>(int column);

class Query {
public:
    Query();
    Query(const std::string& query);
    void SetQuery(const std::string& query);

    void NonQuery();

    ResultSet* GetResults() const;

    inline bool IsOpen() const {
        return this->open;
    }

    void Reset();
    void Close();
private:
    ResultSet results;
    sqlite3_stmt* statement;

    std::string query;
    bool open;
};

// THE FOLLOWING ARE NOT THREAD SAFE !!
// CALL THEM ONLY WHEN MASTER POOL IS INACTIVE
void init_databases();
void close_databases();
}}

#endif

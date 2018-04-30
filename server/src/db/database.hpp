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
    template<typename T>
    T Get(int column, int type);
private:
    ResultSet(Query* query);
    Query* query;
    bool readable;

    friend class Query;
};

/*double ResultSet::Get<double>(int column);
int32_t ResultSet::Get<int32_t>(int column);
int64_t ResultSet::Get<int64_t>(int column);
sosc::time ResultSet::Get<sosc::time>(int column);
std::string ResultSet::Get<std::string>(int column, int type = DB_COL_TEXT);*/

class Query {
public:
    Query();
    Query(const std::string& query);
    void SetQuery(const std::string& query);

    void NonQuery();

    template<typename T>
    T Scalar();
    template<typename T>
    T Scalar(int type);

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

/*double Query::Scalar<double>();
int32_t Query::Scalar<int32_t>();
int64_t Query::Scalar<int64_t>();
sosc::time Query::Scalar<sosc::time>();
std::string Query::Scalar<std::string>(int type = DB_COL_TEXT);*/

// THE FOLLOWING ARE NOT THREAD SAFE !!
// CALL THEM ONLY WHEN MASTER POOL IS INACTIVE
void init_databases();
void close_databases();
}}

#endif

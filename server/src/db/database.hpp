#ifndef SOSC_DATABASE_H
#define SOSC_DATABASE_H

#include "sqlite/sqlite3.h"
#include "../utils/time.hpp"
#include <vector>
#include <string>

#define DB_COL_TEXT 1
#define DB_COL_BLOB 2

#define DB_USE_HARD   1
#define DB_USE_MEMORY 2

namespace sosc {
namespace db {
class Query;

class ResultSet {
public:
    bool IsOpen() const;
    bool Step();

    template<typename T>
    T Get(int column);
    template<typename T>
    T Get(int column, int type);

    int ColumnCount();
private:
    ResultSet(Query* query);
    Query* query;

    friend class Query;
};

/*
template<> double ResultSet::Get<double>(int column);
template<> int32_t ResultSet::Get<int32_t>(int column);
template<> int64_t ResultSet::Get<int64_t>(int column);
template<> sosc::time ResultSet::Get<sosc::time>(int column);
template<> std::string ResultSet::Get<std::string>(int column, int type);
 */

class Query {
public:
    Query();
    Query(const std::string& query, int db = DB_USE_HARD);
    void SetQuery(const std::string& query, int db = DB_USE_HARD);

    template<typename T>
    void Bind(T value, int i);
    template<typename T>
    void Bind(const T& value, int i, int type);

    void NonQuery();

    template<typename T>
    T Scalar();
    template<typename T>
    T Scalar(int type);

    ResultSet* GetResults() const;
    inline bool IsOpen() const {
        return this->open;
    }

    void Reset(bool clearBinds = true);
    void Close();
private:
    ResultSet results;
    sqlite3_stmt* statement;
    sqlite3* database;
    bool open;

    friend class ResultSet;
};

/*
template<> void Query::Bind<double>(double value, int i);
template<> void Query::Bind<int32_t>(int32_t value, int i);
template<> void Query::Bind<int64_t>(int64_t value, int i);
template<> void Query::Bind<sosc::time>(sosc::time value, int i);
template<> std::string Query::Bind<std::string>
    (const std::string& value, int i, int type);

template<> double Query::Scalar<double>();
template<> int32_t Query::Scalar<int32_t>();
template<> int64_t Query::Scalar<int64_t>();
template<> sosc::time Query::Scalar<sosc::time>();
template<> std::string Query::Scalar<std::string>(int type);
*/

// THE FOLLOWING ARE NOT THREAD SAFE !!
// CALL THEM ONLY WHEN MASTER POOL IS INACTIVE
void init_databases();
void close_databases();
}}

#endif

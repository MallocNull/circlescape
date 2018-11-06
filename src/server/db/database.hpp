#ifndef SOSC_DATABASE_H
#define SOSC_DATABASE_H

#include "sqlite/sqlite3.h"
#include "utils/time.hpp"
#include "crypto/sha1.hpp"
#include <vector>
#include <string>

#define DB_USE_HARD   1
#define DB_USE_MEMORY 2

namespace sosc {
namespace db {

class Query;
typedef std::vector<db::Query*> Queries;

class ResultSet {
public:
    bool IsOpen() const;
    bool Step();

    double GetDouble(int column);
    int32_t GetInt32(int column);
    int64_t GetInt64(int column);
    sosc::time GetTime(int column);
    std::string GetText(int column);
    std::string GetBlob(int column);

    int ColumnCount();
private:
    explicit ResultSet(Query* query);
    Query* query;

    friend class Query;
};

class Query {
public:
    Query();
    Query(const Query& query);

    Query& operator= (const Query&) = delete;
    explicit Query(const std::string& query, int db = DB_USE_HARD);
    void SetQuery(const std::string& query, int db = DB_USE_HARD);

    void BindDouble(double value, int i);
    void BindInt32(int32_t value, int i);
    void BindInt64(int64_t value, int i);
    void BindTime(sosc::time value, int i);
    void BindText(const std::string& value, int i);
    void BindBlob(const std::string& value, int i);

    void NonQuery();
    static void NonQuery(const std::string& query);

    double ScalarDouble();
    int32_t ScalarInt32();
    int64_t ScalarInt64();
    sosc::time ScalarTime();
    std::string ScalarText();
    std::string ScalarBlob();

    static double ScalarDouble(const std::string& query);
    static int32_t ScalarInt32(const std::string& query);
    static int64_t ScalarInt64(const std::string& query);
    static sosc::time ScalarTime(const std::string& query);
    static std::string ScalarText(const std::string& query);
    static std::string ScalarBlob(const std::string& query);

    ResultSet* GetResults();
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

// THE FOLLOWING ARE NOT THREAD SAFE !!
// CALL THEM ONLY WHEN MASTER POOL IS INACTIVE
bool init_databases(std::string* error);
void close_databases();
}}

#endif

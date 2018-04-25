#ifndef SOSC_DATABASE_H
#define SOSC_DATABASE_H

#include "sqlite/sqlite3.h"
#include <vector>
#include <string>

namespace sosc {
namespace db {
class ResultSet {
public:

private:

};

class Query {
public:
    Query();
    Query(const std::string& query);
    void SetQuery(const std::string& query);

    void Reset();
    void Close();
private:
    std::string query;
    bool open;
};

// THE FOLLOWING ARE NOT THREAD SAFE !!
// CALL THEM ONLY WHEN MASTER POOL IS INACTIVE
void init_databases();
void close_databases();
}}

#endif

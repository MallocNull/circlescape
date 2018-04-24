#ifndef SOSC_DATABASE_H
#define SOSC_DATABASE_H

#include "sqlite/sqlite3.h"
#include <vector>

namespace sosc {
namespace db {
class ResultSet {
public:

private:

};

class Query {
public:
    template<typename T>
    static T ScalarOnce();
    static void NonQueryOnce();
    static ResultSet Once();


private:

};

// THE FOLLOWING ARE NOT THREAD SAFE !!
// CALL THEM ONLY WHEN MASTER POOL IS INACTIVE
void init_databases();
void close_databases();
}}

#endif

#include "database.hpp"
#include "_init_sql.cpp"

static struct {
    bool ready = false;

    sqlite3* mem_db;
    sqlite3* hard_db;
} _ctx;

void sosc::db::init_databases() {
    if(_ctx.ready)
        return;

    sqlite3_open(":memory:", &_ctx.mem_db);
    sqlite3_exec(_ctx.mem_db, _mem_db_sql, nullptr, nullptr, nullptr);

    sqlite3_open("scape.db", &_ctx.hard_db);
    db::Query query("SELECT * FROM MIGRATIONS ORDER BY ID ASC");
    auto results = query.GetResults();
    while()

    _ctx.ready = true;
}

void sosc::db::close_databases() {
    if(!_ctx.ready)
        return;

    sqlite3_close(_ctx.mem_db);
    sqlite3_close(_ctx.hard_db);
    _ctx.ready = false;
}

sosc::db::Query::Query() : results(this) {
    this->open = false;
}

sosc::db::Query::Query(const std::string& query) : results(this) {
    this->open = false;
    this->SetQuery(query);
}

void sosc::db::Query::SetQuery(const std::string &query) {
    this->Close();

    this->open = true;
}

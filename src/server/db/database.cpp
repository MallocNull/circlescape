#include "database.hpp"
#include "_init_sql.hpp"

static struct {
    bool ready = false;

    sqlite3* mem_db;
    sqlite3* hard_db;
} _ctx;

bool sosc::db::init_databases(std::string* error) {
    if(_ctx.ready)
        return true;

    sqlite3_open(":memory:", &_ctx.mem_db);
    sqlite3_exec(_ctx.mem_db, _mem_db_sql, nullptr, nullptr, nullptr);

    sqlite3_open("scape.db", &_ctx.hard_db);

    int32_t migrationsExist = db::Query::ScalarInt32(
        "SELECT COUNT(*) FROM SQLITE_MASTER WHERE TBL_NAME = 'MIGRATIONS'"
    );
    if(migrationsExist == 0)
        db::Query::NonQuery(_hard_db_init_migration_sql);

    int32_t lastMig = db::Query::ScalarInt32("SELECT MAX(ID) FROM MIGRATIONS");
    if(lastMig > _hard_db_sql.size()) {
        *error = "HARD DB: RECORDED MIGRATION COUNT TOO HIGH";
        return false;
    }

    int id;
    Query insertMigration(
        "INSERT INTO MIGRATIONS (ID, SQL_HASH, DATE_RAN) "
        "VALUES (?, ?, NOW())"
    );
    Query getMigration("SELECT SQL_HASH FROM MIGRATIONS WHERE ID = ?");
    for(id = 0; id < _hard_db_sql.size(); ++id) {
        getMigration.BindInt32(id, 0);

        std::string hash = getMigration.ScalarText();
        if(hash.empty()) {
            if(id < lastMig) {
                *error = "HARD DB: MIGRATION RECORDS NOT CONTINUOUS";
                return false;
            }

            Query::NonQuery(_hard_db_sql[id]);

            insertMigration.BindInt32(id, 0);
            insertMigration.BindText(cgc::sha1(_hard_db_sql[id]), 1);
            insertMigration.NonQuery();
        } else {
            if(hash != cgc::sha1(_hard_db_sql[id])) {
                *error = "HARD DB: MIGRATION SQL HASH MISMATCH";
                return false;
            }
        }

        insertMigration.Reset();
        getMigration.Reset();
    }

    insertMigration.Close();
    getMigration.Close();

    _ctx.ready = true;
    return true;
}

void sosc::db::close_databases() {
    if(!_ctx.ready)
        return;

    sqlite3_close(_ctx.mem_db);
    sqlite3_close(_ctx.hard_db);
    _ctx.ready = false;
}

// ************ //
//  QUERY CODE  //
// ************ //

sosc::db::Query::Query() : results(this) {
    this->open = false;
}

sosc::db::Query::Query(const Query &query) : results(this) {
    this->open = false;
    if(query.open) {
        this->SetQuery(
            sqlite3_sql(query.statement),
            query.database == _ctx.mem_db ? DB_USE_MEMORY : DB_USE_HARD
        );
    }
}

sosc::db::Query::Query(const std::string& query, int db) : results(this) {
    this->open = false;
    this->SetQuery(query, db);
}

void sosc::db::Query::SetQuery(const std::string &query, int db) {
    if(!_ctx.ready)
        return;
    if(!this->open)
        this->Close();

    this->database = db == DB_USE_MEMORY ? _ctx.mem_db : _ctx.hard_db;
    int status = sqlite3_prepare_v2(
        this->database,
        query.c_str(),
        query.length() + 1,
        &this->statement,
        nullptr
    );

    if(status == SQLITE_OK)
        this->open = true;
}

void sosc::db::Query::BindDouble(double value, int i) {
    sqlite3_bind_double(this->statement, i, value);
}

void sosc::db::Query::BindInt32(int32_t value, int i) {
    sqlite3_bind_int(this->statement, i, value);
}

void sosc::db::Query::BindInt64(int64_t value, int i) {
    sqlite3_bind_int64(this->statement, i, value);
}

void sosc::db::Query::BindTime(sosc::time value, int i) {
    sqlite3_bind_int64(this->statement, i, clk::to_unix_time(value));
}

void sosc::db::Query::BindText(const std::string& value, int i) {
    sqlite3_bind_text(
        this->statement, i,
        value.c_str(), -1,
        SQLITE_TRANSIENT
    );
}

void sosc::db::Query::BindBlob(const std::string& value, int i) {
    sqlite3_bind_blob(
        this->statement, i,
        value.c_str(), value.length(),
        SQLITE_TRANSIENT
    );
}

void sosc::db::Query::NonQuery() {
    if(!_ctx.ready || !this->open)
        return;

    this->results.Step();
}

void sosc::db::Query::NonQuery(const std::string &query) {
    if(!_ctx.ready)
        return;

    Query q(query);
    q.NonQuery();
    q.Close();
}

double sosc::db::Query::ScalarDouble() {
    if(!_ctx.ready || !this->open)
        return 0;

    if(this->results.Step())
        return this->results.GetDouble(0);
    else
        return 0;
}

int32_t sosc::db::Query::ScalarInt32() {
    if(!_ctx.ready || !this->open)
        return 0;

    if(this->results.Step())
        return this->results.GetInt32(0);
    else
        return 0;
}

int64_t sosc::db::Query::ScalarInt64() {
    if(!_ctx.ready || !this->open)
        return 0;

    if(this->results.Step())
        return this->results.GetInt64(0);
    else
        return 0;
}

sosc::time sosc::db::Query::ScalarTime() {
    if(!_ctx.ready || !this->open)
        return sosc::time::min();

    if(this->results.Step())
        return this->results.GetTime(0);
    else
        return sosc::time::min();
}

std::string sosc::db::Query::ScalarText() {
    if(!_ctx.ready || !this->open)
        return "";

    if(this->results.Step())
        return this->results.GetText(0);
    else
        return "";
}

std::string sosc::db::Query::ScalarBlob() {
    if(!_ctx.ready || !this->open)
        return "";

    if(this->results.Step())
        return this->results.GetBlob(0);
    else
        return "";
}

double sosc::db::Query::ScalarDouble(const std::string &query) {
    if(!_ctx.ready)
        return 0;

    Query q(query);
    double result = q.ScalarDouble();
    q.Close();

    return result;
}

int32_t sosc::db::Query::ScalarInt32(const std::string &query) {
    if(!_ctx.ready)
        return 0;

    Query q(query);
    int32_t result = q.ScalarInt32();
    q.Close();

    return result;
}

int64_t sosc::db::Query::ScalarInt64(const std::string &query) {
    if(!_ctx.ready)
        return 0;

    Query q(query);
    int64_t result = q.ScalarInt64();
    q.Close();

    return result;
}

sosc::time sosc::db::Query::ScalarTime(const std::string &query) {
    if(!_ctx.ready)
        return sosc::time::min();

    Query q(query);
    sosc::time result = q.ScalarTime();
    q.Close();

    return result;
}

std::string sosc::db::Query::ScalarText(const std::string &query) {
    if(!_ctx.ready)
        return "";

    Query q(query);
    std::string result = q.ScalarText();
    q.Close();

    return result;
}

std::string sosc::db::Query::ScalarBlob(const std::string &query) {
    if(!_ctx.ready)
        return "";

    Query q(query);
    std::string result = q.ScalarBlob();
    q.Close();

    return result;
}

sosc::db::ResultSet* sosc::db::Query::GetResults() {
    return &this->results;
}

void sosc::db::Query::Reset(bool clearBinds) {
    if(clearBinds)
        sqlite3_clear_bindings(this->statement);

    sqlite3_reset(this->statement);
}

void sosc::db::Query::Close() {
    sqlite3_finalize(this->statement);
    this->open = false;
}

// ***************** //
//  RESULT SET CODE  //
// ***************** //

sosc::db::ResultSet::ResultSet(sosc::db::Query *query) {
    this->query = query;
}

bool sosc::db::ResultSet::IsOpen() const {
    return this->query->IsOpen();
}

bool sosc::db::ResultSet::Step() {
    int result = sqlite3_step(this->query->statement);

    if(result == SQLITE_ROW)
        return true;
    else if(result == SQLITE_DONE)
        return false;
    else
        throw std::string(sqlite3_errmsg(this->query->database));
}

double sosc::db::ResultSet::GetDouble(int column) {
    return sqlite3_column_double(this->query->statement, column);
}

int32_t sosc::db::ResultSet::GetInt32(int column) {
    return sqlite3_column_int(this->query->statement, column);
}

int64_t sosc::db::ResultSet::GetInt64(int column) {
    return sqlite3_column_int64(this->query->statement, column);
}

sosc::time sosc::db::ResultSet::GetTime(int column) {
    return clk::from_unix_time(
        sqlite3_column_int64(this->query->statement, column)
    );
}

std::string sosc::db::ResultSet::GetText(int column) {
    auto data = (const char*)
        sqlite3_column_text(this->query->statement, column);

    return std::string(
        data, sqlite3_column_bytes(this->query->statement, column)
    );
}

std::string sosc::db::ResultSet::GetBlob(int column) {
    auto data = (const char*)
        sqlite3_column_blob(this->query->statement, column);

    return std::string(
        data, sqlite3_column_bytes(this->query->statement, column)
    );
}

int sosc::db::ResultSet::ColumnCount() {
    return sqlite3_column_count(this->query->statement);
}
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
    //while()

    _ctx.ready = true;
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

void sosc::db::Query::Bind<double>(double value, int i) {
    sqlite3_bind_double(this->statement, i, value);
}

void sosc::db::Query::Bind<int32_t>(int32_t value, int i) {
    sqlite3_bind_int(this->statement, i, value);
}

void sosc::db::Query::Bind<int64_t>(int64_t value, int i) {
    sqlite3_bind_int64(this->statement, i, value);
}

void sosc::db::Query::Bind<sosc::time>(sosc::time value, int i) {
    sqlite3_bind_int64(this->statement, i, clk::to_unix_time(value));
}

void sosc::db::Query::Bind<std::string>
    (const std::string& value, int i, int type)
{
    if(type == DB_COL_TEXT)
        sqlite3_bind_text(
            this->statement, i,
            value.c_str(), -1,
            SQLITE_TRANSIENT
        );
    else
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

double sosc::db::Query::Scalar<double>() {
    if(!_ctx.ready || !this->open)
        return 0;

    if(this->results.Step())
        return this->results.Get<double>(0);
    else
        return 0;
}

int32_t sosc::db::Query::Scalar<int32_t>() {
    if(!_ctx.ready || !this->open)
        return 0;

    if(this->results.Step())
        return this->results.Get<int32_t>(0);
    else
        return 0;
}

int64_t sosc::db::Query::Scalar<int64_t>() {
    if(!_ctx.ready || !this->open)
        return 0;

    if(this->results.Step())
        return this->results.Get<int64_t>(0);
    else
        return 0;
}

sosc::time sosc::db::Query::Scalar<sosc::time>() {
    if(!_ctx.ready || !this->open)
        return sosc::time::min();

    if(this->results.Step())
        return this->results.Get<sosc::time>(0);
    else
        return sosc::time::min();
}

std::string sosc::db::Query::Scalar<std::string>(int type) {
    if(!_ctx.ready || !this->open)
        return "";

    if(this->results.Step())
        return this->results.Get<std::string>(0, type);
    else
        return "";
}

sosc::db::ResultSet* sosc::db::Query::GetResults() const {
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

double sosc::db::ResultSet::Get<double>(int column) {
    return sqlite3_column_double(this->query->statement, column);
}

int32_t sosc::db::ResultSet::Get<int32_t>(int column) {
    return sqlite3_column_int(this->query->statement, column);
}

int64_t sosc::db::ResultSet::Get<int64_t>(int column) {
    return sqlite3_column_int64(this->query->statement, column);
}

sosc::time sosc::db::ResultSet::Get<sosc::time>(int column) {
    return clk::from_unix_time(
        sqlite3_column_int64(this->query->statement, column)
    );
}

std::string sosc::db::ResultSet::Get<std::string>(int column, int type) {
    auto data = (const char*)
        (type == DB_COL_TEXT
            ? sqlite3_column_text(this->query->statement, column)
            : sqlite3_column_blob(this->query->statement, column));

    return std::string(
        data, sqlite3_column_bytes(this->query->statement, column)
    );
}

int sosc::db::ResultSet::ColumnCount() {
    return sqlite3_column_count(this->query->statement);
}
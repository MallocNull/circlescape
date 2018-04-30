#include <vector>

const char* _mem_db_sql =
    "CREATE TABLE SERVER_LIST ( "
    ");";

const std::vector<const char*> _hard_db_sql = {
    /** START MIGRATION 0 **/
    "CREATE TABLE `MIGRATIONS` ("
        "`ID` INTEGER NOT NULL,"
        "`SQL_HASH` TEXT NOT NULL,"
        "`DATE_RAN` INTEGER NOT NULL,"
        "PRIMARY KEY(`ID`)"
    ") WITHOUT ROWID;\n"

    "CREATE TABLE `USERS` ("
        "`ID` INTEGER PRIMARY KEY AUTOINCREMENT,"
        "`USERNAME` TEXT NOT NULL,"
        "`PASS_HASH` TEXT NOT NULL,"
        "`EMAIL` TEXT NOT NULL,"
        "`ACTIVATED` INTEGER NOT NULL DEFAULT 0,"
        "`JOINED` INTEGER NOT NULL"
    ");\n"

    "CREATE TABLE `ACTIVATION_CODES` ("
        "`USER_ID` INTEGER NOT NULL UNIQUE,"
        "`CODE` TEXT NOT NULL,"
        "`CREATED` INTEGER NOT NULL,"
        "PRIMARY KEY(`USER_ID`)"
    ") WITHOUT ROWID;\n",
    /** END MIGRATION 0 **/
};
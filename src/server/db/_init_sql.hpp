#ifndef SOSC_INIT_SQL_DEF
#define SOSC_INIT_SQL_DEF

#include <vector>

const std::vector<const char*> _mem_db_sql = {
    "CREATE TABLE `SERVER_LIST` ("
        "`ID` INTEGER PRIMARY KEY AUTOINCREMENT,"
        "`NAME` TEXT NOT NULL,"
        "`LICENSE` TEXT NOT NULL,"
        "`IP_ADDR` TEXT NOT NULL,"
        "`PORT` INTEGER NOT NULL,"
        "`USERS` INTEGER NOT NULL DEFAULT 0,"
        "`MAX_USERS` INTEGER NOT NULL DEFAULT 0"
    ");",

    "CREATE TABLE `USER_KEYS` ("
        "`ID` INTEGER,"
        "`SECRET` BLOB NOT NULL UNIQUE,"
        "PRIMARY KEY(`ID`)"
    ");",
};

const std::vector<const char*> _hard_db_init_migration_sql = {
    "CREATE TABLE `MIGRATIONS` ("
        "`ID` INTEGER NOT NULL,"
        "`SQL_HASH` TEXT NOT NULL,"
        "`DATE_RAN` INTEGER NOT NULL,"
        "PRIMARY KEY(`ID`)"
    ") WITHOUT ROWID;",
};

const std::vector<std::vector<const char*>> _hard_db_sql = {
    {  /** START MIGRATION 0 **/
        "CREATE TABLE `SERVER_LICENSES` ("
            "`KEY_ID` INTEGER PRIMARY KEY AUTOINCREMENT,"
            "`SECRET` BLOB NOT NULL,"
            "`ALLOWANCE` INTEGER NOT NULL DEFAULT 0"
        ");",

        "CREATE UNIQUE INDEX `UIX_SERVER_LICENSES` ON `SERVER_LICENSES` ("
            "`KEY_ID`, `SECRET`"
        ");",

        "CREATE TABLE `USERS` ("
            "`ID` INTEGER PRIMARY KEY AUTOINCREMENT,"
            "`USERNAME` TEXT NOT NULL,"
            "`PASS_HASH` TEXT NOT NULL,"
            "`EMAIL` TEXT NOT NULL,"
            "`ACTIVATED` INTEGER NOT NULL DEFAULT 0,"
            "`JOINED` INTEGER NOT NULL"
        ");",

        "CREATE TABLE `ACTIVATION_CODES` ("
            "`USER_ID` INTEGER NOT NULL UNIQUE,"
            "`CODE` TEXT NOT NULL,"
            "`CREATED` INTEGER NOT NULL,"
            "PRIMARY KEY(`USER_ID`)"
        ") WITHOUT ROWID;",
    }, /** END MIGRATION 0 **/
};

#endif
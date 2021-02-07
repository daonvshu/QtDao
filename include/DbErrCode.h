#pragma once

class DbErrCode {
public:
    enum Code {
        ERR_NOT_SET = 0,
        //connect fail
        SQLITE_CREATE_DB_PATH_FAIL = 0x1001,

        MYSQL_CONNECT_FAIL = 0x2001,

        SQLSERVER_CREATE_DB_PATH_FAIL = 0x4001,
        SQLSERVER_CONNECT_FAIL = 0x4002,
        //init fail code
        SQLITE_CREATE_TABLE_FAIL = 0x1004,
        SQLITE_CREATE_INDEX_FAIL = 0x1008,
        SQLITE_CREATE_TMP_TABLE_FAIL = 0x1010,
        SQLITE_DUMP_FIELD_FAIL = 0x1020,
        SQLITE_DROP_OLD_INDEX_FAIL = 0x1040,

        MYSQL_CREATE_DATABASE_FAIL = 0x2004,
        MYSQL_CREATE_TABLE_FAIL = 0x2008,
        MYSQL_CREATE_INDEX_FAIL = 0x2010,
        MYSQL_CREATE_TMP_TABLE_FAIL = 0x2020,
        MYSQL_DUMP_FIELD_FAIL = 0x2040,
        MYSQL_DROP_OLD_INDEX_FAIL = 0x2080,

        SQLSERVER_CREATE_DATABASE_FAIL = 0x4004,
        SQLSERVER_CREATE_TABLE_FAIL = 0x4008,
        SQLSERVER_CREATE_INDEX_FAIL = 0x4010,
        SQLSERVER_CREATE_TMP_TABLE_FAIL = 0x2020,
        SQLSERVER_DUMP_FIELD_FAIL = 0x2040,
        SQLSERVER_DROP_OLD_INDEX_FAIL = 0x2080,

        DATABASE_INIT_FAIL = 0x00fc,
        //connect err
        CREATE_CONNECTION_FAIL = 0x0100,
        //execute fail
        SQL_EXEC_FAIL = 0x0200,
    };

    DbErrCode(Code code) : code(code) {}

    constexpr bool operator==(Code target) {
        return code & target;
    }

    Code code;
};
#pragma once

class DbErrCode {
public:
    enum Code {
        ERR_NOT_SET = 0,
        //connect fail
        SQLITE_CREATE_DB_PATH_FAIL = 0x0001,

        MYSQL_CONNECT_FAIL = 0x0002,
        //init fail code
        SQLITE_CREATE_TABLE_FAIL = 0x4004,
        SQLITE_CREATE_INDEX_FAIL = 0x4008,
        SQLITE_CREATE_TMP_TABLE_FAIL = 0x4010,
        SQLITE_DUMP_FIELD_FAIL = 0x4020,
        SQLITE_DROP_OLD_INDEX_FAIL = 0x4040,

        MYSQL_CREATE_DATABASE_FAIL = 0x8004,
        MYSQL_CREATE_TABLE_FAIL = 0x8008,
        MYSQL_CREATE_INDEX_FAIL = 0x8010,
        MYSQL_CREATE_TMP_TABLE_FAIL = 0x8020,
        MYSQL_DUMP_FIELD_FAIL = 0x8040,
        MYSQL_DROP_OLD_INDEX_FAIL = 0x8080,

        DATABASE_INIT_FAIL = 0xc0fc,
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
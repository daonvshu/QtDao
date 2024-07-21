#pragma once

#include <qobject.h>

#include "datakey.h"

using namespace QDataUtil;

enum class PrintOutputTarget {
    Target_Std,
    Target_Debug_Console,
};

enum class TestTargetDb {
    Target_Sqlite,
    Target_Mysql,
    Target_SqlServer,
    Target_PSql,
};

struct TestOptionCommon : DataDumpInterface {
    DATA_KEY(int, version);
    DATA_KEY(QString, driver);
    DATA_KEY(QString, password);
    DATA_KEY(QString, options);
};

struct TestOptionSqlite : TestOptionCommon {
    DATA_KEY(QString, saveDir);

    QList<DataReadInterface *> prop() override {
        return { &version, &driver, &password, &options,
                 &saveDir
        };
    }
};

struct TestOptionMySql : TestOptionCommon {
    DATA_KEY(QString, host);
    DATA_KEY(QString, user);
    DATA_KEY(int, port);

    QList<DataReadInterface *> prop() override {
        return { &version, &driver, &password, &options,
                 &host, &user, &port
        };
    }
};

struct TestOptionSqlServer : TestOptionMySql {

};

struct TestOptionPSql : TestOptionMySql {

};

struct TestConfigData : DataDumpInterface {

    DATA_KEY(QString, printTaget);
    DATA_KEY(bool, systemCodePage);

    DATA_KEY(QString, targetDb);

    DATA_KEY(TestOptionSqlite, optionSqlite);
    DATA_KEY(TestOptionMySql, optionMysql);
    DATA_KEY(TestOptionSqlServer, optionSqlServer);
    DATA_KEY(TestOptionPSql, optionPSql);

    PrintOutputTarget getOutputTarget() const {
        if (printTaget() == "debugConsole") {
            return PrintOutputTarget::Target_Debug_Console;
        }
        return PrintOutputTarget::Target_Std;
    }

    TestTargetDb getTestTargetDb() const {
        auto configStr = targetDb().toLower();
        if (configStr == "mysql") {
            return TestTargetDb::Target_Mysql;
        }
        if (configStr == "sqlserver") {
            return TestTargetDb::Target_SqlServer;
        }
        if (configStr == "psql") {
            return TestTargetDb::Target_PSql;
        }
        return TestTargetDb::Target_Sqlite;
    }

    QList<DataReadInterface *> prop() override {
        return { &printTaget, &systemCodePage, &targetDb,
                 &optionSqlite, &optionMysql, &optionSqlServer, &optionPSql
        };
    }
};

class TestConfigLoader {
public:
    static TestConfigLoader& instance();

    const TestConfigData& config() const {
        return configData;
    }

private:
    TestConfigLoader();

private:
    TestConfigData configData;
};

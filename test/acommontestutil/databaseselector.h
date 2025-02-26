#pragma once

#include "dao.h"

#include <qdir.h>
#include <qdebug.h>

#include "utils/testconfigloader.h"

class DatabaseSelector {
public:
    explicit DatabaseSelector(TestTargetDb targetDb): targetDb(targetDb) {}

    void setupDatabase(const QString& databaseName, int version = -1) {
        try {
            if (targetDb == TestTargetDb::Target_Sqlite) {
                const auto& configOption = TestConfigLoader::instance().config().optionSqlite();
                dao::_config<dao::ConfigSqliteBuilder>()
                    .version(version == -1 ? configOption.version() : version)
                    .databaseName(databaseName)
                    //.driver("SQLITECIPHER")
                    //.password("H*u1RhdJW&tbmHCU*FPZ#58KcL1q@ZXP")
                    .options(configOption.options())
                    .saveDir(configOption.saveDir() == "current" ? QDir::currentPath() : configOption.saveDir())
                    .initializeDatabase();
            } else if (targetDb == TestTargetDb::Target_Mysql) {
                const auto& configOption = TestConfigLoader::instance().config().optionMysql();
                dao::_config<dao::ConfigMysqlBuilder>()
                    .version(version == -1 ? configOption.version() : version)
                    .databaseName(databaseName)
                    .host(configOption.host())
                    .port(configOption.port())
                    .user(configOption.user())
                    .password(configOption.password())
                    .options(configOption.options())
                    .initializeDatabase();
            } else if (targetDb == TestTargetDb::Target_SqlServer) {
                const auto& configOption = TestConfigLoader::instance().config().optionSqlServer();
                dao::_config<dao::ConfigSqlServerBuilder>()
                    .version(version == -1 ? configOption.version() : version)
                    .databaseName(databaseName)
                    .host(configOption.host())
                    .user(configOption.user())
                    .password(configOption.password())
                    .options(configOption.options())
                    .initializeDatabase();
            } else if (targetDb == TestTargetDb::Target_PSql) {
                const auto& configOption = TestConfigLoader::instance().config().optionPSql();
                dao::_config<dao::ConfigPSqlBuilder>()
                    .version(version == -1 ? configOption.version() : version)
                    .databaseName(databaseName)
                    .host(configOption.host())
                    .port(configOption.port())
                    .user(configOption.user())
                    .password(configOption.password())
                    .options(configOption.options())
                    .initializeDatabase();
            }
        } catch (dao::DaoException& e) {
            Q_UNUSED(e)
            auto validDrivers = QSqlDatabase::drivers();
            Q_UNUSED(validDrivers)
            qFatal("setup database fail!");
        }
    }

protected:
    TestTargetDb targetDb;
};

#define PASSSQLITE if (targetDb == TestTargetDb::Target_Sqlite) return
#define PASSMYSQL if (targetDb == TestTargetDb::Target_Mysql) return
#define PASSSQLSERVER if (targetDb == TestTargetDb::Target_SqlServer) return
#define PASSPSQL if (targetDb == TestTargetDb::Target_PSql) return
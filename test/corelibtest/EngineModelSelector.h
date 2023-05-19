#pragma once

#include "dao.h"

#include <qdir.h>

enum EngineModel {
    Engine_Sqlite = 0,
    Engine_Mysql,
    Engine_SqlServer
};

class EngineModelSelector {
public:
    EngineModelSelector(EngineModel engineModel) {
        this->engineModel = engineModel;
    }

    void setupDatabase(int version = 1) {
        if (engineModel == Engine_Sqlite) {
            dao::_config<dao::ConfigSqliteBuilder>()
                    .version(version)
                    .databaseName("sqlitetest")
                    //.driver("SQLITECIPHER")
                    //.password("H*u1RhdJW&tbmHCU*FPZ#58KcL1q@ZXP")
                    .options("QSQLITE_BUSY_TIMEOUT=100")
                    .saveDir(QDir::currentPath())
                    .initializeDatabase();
        } else if (engineModel == Engine_Mysql) {
            dao::_config<dao::ConfigMysqlBuilder>()
                    .version(version)
                    .databaseName("mysqltest")
                    .host("localhost")
                    .port(3306)
                    .user("root")
                    .password("root")
                    .options("MYSQL_OPT_CONNECT_TIMEOUT=3;MYSQL_OPT_READ_TIMEOUT=3;MYSQL_OPT_WRITE_TIMEOUT=3")
                    .initializeDatabase();
        } else if (engineModel == Engine_SqlServer) {
            dao::_config<dao::ConfigSqlServerBuilder>()
                    .version(version)
                    .databaseName("sqlservertest")
                    .host("localhost")
                    .user("sa")
                    .password("root")
                    .initializeDatabase();
        }
    }

protected:
    EngineModel engineModel;
};

#define PASSSQLITE if (engineModel == Engine_Sqlite) return
#define PASSMYSQL if (engineModel == Engine_Mysql) return
#define PASSSQLSERVER if (engineModel == Engine_SqlServer) return
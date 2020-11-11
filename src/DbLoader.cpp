﻿#include "DbLoader.h"

#include <qdom.h>
#include <qfile.h>
#include <qdir.h>
#include <qstandardpaths.h>

#include "dbclients/SqliteClient.h"
#include "dbclients/MysqlClient.h"
#include "dbclients/SqlServerClient.h"

void DbLoader::loadConfig(const QString& configPath, DbExceptionHandler* exceptionHandler) {
    DbExceptionHandler::setExceptionHandler(exceptionHandler);

    QDomDocument xmlDoc;
    QFile file(configPath);
    bool s = file.open(QIODevice::ReadOnly);
    Q_ASSERT_X(s, "Dbloader::loadConfig", "open config file fail!");
    s = xmlDoc.setContent(&file);
    Q_ASSERT_X(s, "DbLoader::loadCondigure", "dom set content fail!");
    file.close();

    auto root = xmlDoc.documentElement();
    Q_ASSERT(root.nodeName() == "dao");

    auto sqltype = root.attribute("type");
    if (sqltype == "sqlite") {
        config.dbType = "QSQLITE";
        sqlClient = new SqliteClient;
    }
    else if (sqltype == "mysql") {
        config.dbType = "QMYSQL";
        sqlClient = new MysqlClient;
    } 
    else if (sqltype == "sqlserver") {
        config.dbType = "QODBC";
        sqlClient = new SqlServerClient;
    }
    Q_ASSERT(!config.dbType.isEmpty());

    config.version = root.attribute("version").toInt();
    config.dbName = root.attribute("dbname");

    auto options = root.childNodes();
    for (int i = 0; i < options.count(); i++) {
        auto c = options.at(i).toElement();
        if (c.tagName() == "dbname") {
            config.dbName = c.text();
        } else if (c.tagName() == "dbhost") {
            config.dbHost = c.text();
        } else if (c.tagName() == "dbuname") {
            config.dbUName = c.text();
        } else if (c.tagName() == "dbpcc") {
            config.dbPcc = c.text();
        } else if (c.tagName() == "dbport") {
            config.dbPort = c.text().toInt();
        } else if (c.tagName() == "dboption") {
            config.dbOption = c.text();
        }
    }
}

void DbLoader::init() {
    try {
        init_priv();
    } catch (const char* reason) {
        DbExceptionHandler::exceptionHandler->initDbFail(reason);
    }
}

void DbLoader::init(const QString& configPath, DbExceptionHandler* exceptionHandler) {
    loadConfig(configPath, exceptionHandler);
    init();
}

void DbLoader::init_priv() {
    sqlClient->testConnect();
    sqlClient->createDatabase();
}

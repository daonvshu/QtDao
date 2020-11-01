#include "DbLoader.h"

#include <qdom.h>
#include <qfile.h>
#include <qdir.h>
#include <qstandardpaths.h>

class SqliteInitClient : public DbInitClient {
public:
    void testConnect() {
        auto appLocal = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        QDir dir;
        if (!dir.exists(appLocal)) {
            if (!dir.mkdir(appLocal)) {
                throw "cannot create sqlite store path!";
            }
        }
    }

    void createDatabase() {

    }
};

class MysqlInitClient : public DbInitClient {
public:
    void testConnect() {

    }

    void createDatabase() {

    }
};

class SqlServerClient : public DbInitClient {
public:
    void testConnect() {

    }

    void createDatabase() {

    }
};

void DbLoader::loadConfig(const QString& configPath, DbExceptionHandler* exceptionHandler) {
    if (exceptionHandler != nullptr) {
        delete DbExceptionHandler::exceptionHandler; //delete default hander
        DbExceptionHandler::exceptionHandler = exceptionHandler;
    }

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
        initClient = new SqliteInitClient;
    }
    else if (sqltype == "mysql") {
        config.dbType = "QMYSQL";
        initClient = new MysqlInitClient;
    } 
    else if (sqltype == "sqlserver") {
        config.dbType = "QODBC";
        initClient = new SqlServerClient;
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
    initClient->testConnect();
    initClient->createDatabase();
}

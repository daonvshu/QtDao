#include "DbLoader.h"

#include <qdom.h>
#include <qfile.h>
#include <qdir.h>
#include <qstandardpaths.h>

#include "dbclients/SqliteClient.h"
#include "dbclients/MysqlClient.h"
#include "dbclients/SqlServerClient.h"

#include "query/BaseQuery.h"

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
    config.versionValid = true;

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
    } catch (DaoException& e) {
        DbExceptionHandler::exceptionHandler->initDbFail(e.reason);
    }
}

void DbLoader::init(const QString& configPath, DbExceptionHandler* exceptionHandler) {
    loadConfig(configPath, exceptionHandler);
    init();
}

void DbLoader::init_priv() {
    sqlClient->testConnect();
    sqlClient->createDatabase();

    invokeCreateTables();

    int localversion = getLocalVersion();
    if (localversion != -1) {
        config.versionValid = localversion >= config.version;
    } else {
        updateLocalVersion();
    }

    if (!config.versionValid) {
        try {
            invokeTableUpgrade();
            updateLocalVersion();
        } catch (DaoException& e) {
            DbExceptionHandler::exceptionHandler->upgradeFail(e.reason);
        }
    }
}

void DbLoader::updateLocalVersion() {
    getClient().createTableIfNotExist("dao_version", QStringList() << "ver int", QStringList());
    bool dataRowExist = false;
    auto query = BaseQuery::queryPrimitiveThrowable("select count(*) from dao_version");
    int count = 0;
    if (query.next()) {
        count = query.value(0).toInt();
    }
    if (count == 0) {
        BaseQuery::queryPrimitiveThrowable(QString("insert into dao_version(ver) values(%1)").arg(config.version));
    } else {
        BaseQuery::queryPrimitiveThrowable(QString("update dao_version set ver = %1").arg(config.version));
    }
}

int DbLoader::getLocalVersion() {
    if (!getClient().checkTableExist("dao_version")) {
        return -1;
    }
    int version = -1;
    auto query = BaseQuery::queryPrimitiveThrowable("select *from dao_version");
    if (query.next()) {
        version = query.value(0).toInt();
    }
    return version;
}

void DbLoader::invokeCreateTables() {
    const auto metaObj = QMetaType::metaObjectForType(QMetaType::type(getDelegateStr()));
    Q_ASSERT_X(metaObj != nullptr, "DbLoader::init", "use DbEntityGenerator to create Entity");
    QObject* obj = metaObj->newInstance();
    metaObj->invokeMethod(obj, "createEntityTables");
    delete obj;
}

void DbLoader::invokeTableUpgrade() {
    const auto metaObj = QMetaType::metaObjectForType(QMetaType::type(getDelegateStr()));
    Q_ASSERT_X(metaObj != nullptr, "DbLoader::init", "use DbEntityGenerator to create Entity");
    QObject* obj = metaObj->newInstance();
    metaObj->invokeMethod(obj, "entityTablesUpgrade");
    delete obj;
}

QByteArray DbLoader::getDelegateStr() {
    QByteArray entityDelegate;
    if (config.isSqlite()) {
        entityDelegate = "DaoSqlite::EntityDelegate*";
    } else if (config.isMysql()) {
        entityDelegate = "DaoMysql::EntityDelegate*";
    } else if (config.isSqlServer()) {
        entityDelegate = "DaoSqlServer::EntityDelegate*";
    }
    return entityDelegate;
}

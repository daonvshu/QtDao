#include "DbLoader.h"

#include "dbclients/SqliteClient.h"
#include "dbclients/MysqlClient.h"
#include "dbclients/SqlServerClient.h"

#include "query/BaseQuery.h"

DbConfig DbLoader::config;
AbstractClient* DbLoader::sqlClient = nullptr;

bool DbLoader::init(const QObject& config, DbExceptionHandler* exceptionHandler) {
    DbExceptionHandler::setExceptionHandler(exceptionHandler);
    loadConfig(config);
    return init();
}

void DbLoader::loadConfig(const QObject& cg) {
    auto sqltype = cg.property("type");
    Q_ASSERT(sqltype.isValid());
    config.dbType = sqltype.toString();
    config.dbDriver = cg.property("driver").toString();
    if (config.dbDriver.isEmpty()) {
        if (sqltype == "sqlite") {
            config.dbDriver = "QSQLITE";
        } else if (sqltype == "mysql") {
            config.dbDriver = "QMYSQL";
        } else if (sqltype == "sqlserver") {
            config.dbDriver = "QODBC";
        }
    }

    if (sqltype == "sqlite") {
        sqlClient = new SqliteClient;
    } else if (sqltype == "mysql") {
        sqlClient = new MysqlClient;
    } else if (sqltype == "sqlserver") {
        sqlClient = new SqlServerClient;
    }
    
    config.version = cg.property("version").toInt();

    config.dbName = cg.property("dbName").toString();
    config.dbHost = cg.property("dbHost").toString();
    config.dbUName = cg.property("dbUName").toString();
    config.dbPcc = cg.property("dbPcc").toString();
    config.dbPort = cg.property("dbPort").toInt();
    config.dbOption = cg.property("dbOption").toString();
    config.saveDirectory = cg.property("saveDirectory").toString();
}

bool DbLoader::init() {
    try {
        init_priv();
    } catch (DaoException& e) {
        if (DbExceptionHandler::exceptionHandler) {
            DbExceptionHandler::exceptionHandler->initDbFail(e.code, e.reason);
        }
        Q_ASSERT_X(DbExceptionHandler::exceptionHandler != nullptr, "DbLoader", "database init fail!");
        return false;
    }
    return true;
}

void DbLoader::init_priv() {
    sqlClient->testConnect();

    if (config.createDbEnabled) {
        sqlClient->createDatabase();
    }

    int localversion = getLocalVersion();
    if (localversion != -1) {
        config.versionValid = localversion >= config.version;
    } else {
        updateLocalVersion();
    }

    if (config.createTableEnabled) {
        invokeCreateTables();

        if (!config.versionValid) {
            invokeTableUpgrade();
            updateLocalVersion();
        }
    } else {
        if (!config.versionValid) {
            throw DaoException(DbErrCode::DATABASE_INIT_FAIL, "The local version is smaller than the target version!");
        }
    }
}

void DbLoader::updateLocalVersion() {
    if (config.isSqlite() || config.isSqlServer()) {
        getClient().createTableIfNotExist("dao_version", QStringList() << "ver int", QStringList());
    } else if (config.isMysql()) {
        getClient().createTableIfNotExist("dao_version", QString(), QStringList() << "ver int", QStringList());
    }
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

void DbLoader::disableCreateDatabase() {
    config.createDbEnabled = false;
}

void DbLoader::disableCreateTable() {
    config.createTableEnabled = false;
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
        entityDelegate = "DaoSqlite::SqliteEntityDelegate*";
    } else if (config.isMysql()) {
        entityDelegate = "DaoMysql::MysqlEntityDelegate*";
    } else if (config.isSqlServer()) {
        entityDelegate = "DaoSqlServer::SqlServerEntityDelegate*";
    }
    return entityDelegate;
}

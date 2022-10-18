#include "config/configbuilder.h"

#include "dbclients/mysqlclient.h"
#include "dbclients/sqliteclient.h"
#include "dbclients/sqlserverclient.h"
#include "dbexceptionhandler.h"
#include "query/basequery.h"

QTDAO_BEGIN_NAMESPACE

ConfigBuilder::ConfigBuilder(ConfigType type)
    : configType(type)
{
    switch (type) {
        case ConfigType::Sqlite:
            mDriver = "QSQLITE";
            dbInitClient = QSharedPointer<AbstractClient>(new SqliteClient);
            break;
        case ConfigType::Mysql:
            mDriver = "QMYSQL";
            dbInitClient = QSharedPointer<AbstractClient>(new MysqlClient);
            break;
        case ConfigType::SqlServer:
            mDriver = "QODBC";
            dbInitClient = QSharedPointer<AbstractClient>(new SqlServerClient);
            break;
    }
}

void ConfigBuilder::setupDatabase() {
    dbInitClient->testConnect();

    if (createDbEnabled) {
        dbInitClient->createDatabase();
    }

    bool versionValid = true;
    int localVersion = getLocalVersion();
    if (localVersion != -1) {
        versionValid = mVersion <= localVersion;
    } else {
        updateLocalVersion();
    }

    if (createTableEnabled) {
        invokeCreateTables();

        if (!versionValid) {
            invokeTableUpgrade();
            updateLocalVersion();
        }

    } else {
        if (!versionValid) {
            throw DaoException("The local version is smaller than the target version!");
        }
    }
}

int ConfigBuilder::getLocalVersion() {
    if (!dbInitClient->checkTableExist("dao_version")) {
        return -1;
    }
    int version = -1;
    auto query = BaseQuery::queryPrimitive("select *from dao_version");
    if (query.next()) {
        version = query.value(0).toInt();
    }
    return version;
}

void ConfigBuilder::updateLocalVersion() {
    if (isSqlite() || isSqlServer()) {
        dbInitClient->createTableIfNotExist("dao_version", QStringList() << "ver int", QStringList());
    } else if (isMysql()) {
        dbInitClient->createTableIfNotExist("dao_version", QString(), QStringList() << "ver int", QStringList());
    }
    auto query = BaseQuery::queryPrimitive("select count(*) from dao_version");
    int count = 0;
    if (query.next()) {
        count = query.value(0).toInt();
    }
    if (count == 0) {
        BaseQuery::queryPrimitive(QString("insert into dao_version(ver) values(%1)").arg(mVersion));
    } else {
        BaseQuery::queryPrimitive(QString("update dao_version set ver = %1").arg(mVersion));
    }
}

void ConfigBuilder::invokeCreateTables() {
    const auto metaObj = QMetaType::metaObjectForType(QMetaType::type(getDelegateStr()));
    Q_ASSERT_X(metaObj != nullptr, "DbLoader::init", "use vscode-qtdao to create Entity");
    QObject* obj = metaObj->newInstance();
    QMetaObject::invokeMethod(obj, "createEntityTables");
    delete obj;
}

void ConfigBuilder::invokeTableUpgrade() {
    const auto metaObj = QMetaType::metaObjectForType(QMetaType::type(getDelegateStr()));
    Q_ASSERT_X(metaObj != nullptr, "DbLoader::init", "use vscode-qtdao to create Entity");
    QObject* obj = metaObj->newInstance();
    QMetaObject::invokeMethod(obj, "entityTablesUpgrade");
    delete obj;
}

QByteArray ConfigBuilder::getDelegateStr() {
    QByteArray entityDelegate;
    switch (configType) {
        case ConfigType::Sqlite:
            entityDelegate = "DaoSqlite::SqliteEntityDelegate*";
            break;
        case ConfigType::Mysql:
            entityDelegate = "DaoMysql::MysqlEntityDelegate*";
            break;
        case ConfigType::SqlServer:
            entityDelegate = "DaoSqlServer::SqlServerEntityDelegate*";
            break;
    }
    return entityDelegate;
}

QTDAO_END_NAMESPACE
#include "config/configbuilder.h"
#include "config/configmanager.h"

#include "dbclients/mysqlclient.h"
#include "dbclients/sqliteclient.h"
#include "dbclients/sqlserverclient.h"

#include "dbexception.h"
#include "query/basequery.h"
#include "versionctl/versioncontrol.h"

#include "connectionpool.h"

QTDAO_BEGIN_NAMESPACE

ConfigBuilder::ConfigBuilder(ConfigType type)
    : configType(type)
{
    switch (type) {
        case ConfigType::Sqlite:
            mDriver = "QSQLITE";
            dbClient = QSharedPointer<AbstractClient>(new SqliteClient);
            break;
        case ConfigType::Mysql:
            mDriver = "QMYSQL";
            dbClient = QSharedPointer<AbstractClient>(new MysqlClient);
            break;
        case ConfigType::SqlServer:
            mDriver = "QODBC";
            dbClient = QSharedPointer<AbstractClient>(new SqlServerClient);
            break;
    }
}

void ConfigBuilder::setupDatabase() {
    dbClient->config = this;
    dbClient->testConnect();

    if (dbUpgrader.isNull()) {
        dbUpgrader = QSharedPointer<DatabaseUpgrader>(new DatabaseUpgrader);
    }
    dbUpgrader->setCurClient(dbClient.get());
    dbUpgrader->setCurConfig(this);

    if (createDbEnabled) {
        dbClient->createDatabase();
    }

    if (createTableEnabled) {
        if (mSessionId == -1) {
            VersionControl::checkLocalVersion(mSessionId);
        } else {
            DAO_SCOPED_SESSION_BEGIN(mSessionId);
            VersionControl::checkLocalVersion(mSessionId);
        }
    }
}

int ConfigBuilder::getLocalVersion(qint64 sessionId) {
    return VersionControl::getLocalVersion(sessionId);
}

ConfigBuilder &ConfigBuilder::setDatabaseUpgrader(DatabaseUpgrader *databaseUpgrader) {
    dbUpgrader = QSharedPointer<DatabaseUpgrader>(databaseUpgrader);
    return *this;
}

QSqlDatabase ConfigBuilder::getConnection(const QString &connectionName) {
    return getConnection(connectionName, mDatabaseName);
}

QTDAO_END_NAMESPACE
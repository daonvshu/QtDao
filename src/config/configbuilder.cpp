#include "config/configbuilder.h"

#include "dbclients/mysqlclient.h"
#include "dbclients/sqliteclient.h"
#include "dbclients/sqlserverclient.h"
#include "dbexception.h"
#include "query/basequery.h"
#include "versionctl/versioncontrol.h"

QTDAO_BEGIN_NAMESPACE

QScopedPointer<ConfigBuilder> globalConfig;

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
    globalConfig->dbClient->testConnect();

    if (globalConfig->dbUpgrader.isNull()) {
        globalConfig->dbUpgrader = QSharedPointer<DatabaseUpgrader>(new DatabaseUpgrader);
    }
    globalConfig->dbUpgrader->setCurClient(globalConfig->dbClient.get());
    globalConfig->dbUpgrader->setCurConfig(globalConfig.get());

    if (createDbEnabled) {
        globalConfig->dbClient->createDatabase();
    }

    if (createTableEnabled) {
        VersionControl::checkLocalVersion();
    }
}

int ConfigBuilder::getLocalVersion() {
    return VersionControl::getLocalVersion();
}

ConfigBuilder &ConfigBuilder::setDatabaseUpgrader(DatabaseUpgrader *databaseUpgrader) {
    dbUpgrader = QSharedPointer<DatabaseUpgrader>(databaseUpgrader);
    return *this;
}

QTDAO_END_NAMESPACE
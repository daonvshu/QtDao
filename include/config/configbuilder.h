#pragma once

#include <qobject.h>

#include "../global.h"
#include "../dbclients/abstractclient.h"

#include "configtype.h"

#include <qsqldatabase.h>
#include <qsharedpointer.h>
#include <qscopedpointer.h>

class ConnectionPool;

QTDAO_BEGIN_NAMESPACE

class ConfigBuilder {
public:
    explicit ConfigBuilder(ConfigType type);

    virtual ConfigBuilder& version(int ver) = 0;

    virtual ConfigBuilder& driver(const QString& driver) = 0;

    virtual ConfigBuilder& databaseName(const QString& name) = 0;

    virtual ConfigBuilder& configAlias(const QString& alias) = 0;

    virtual ConfigBuilder& session(qint64 sessionId) = 0;

    virtual ConfigBuilder& password(const QString& password) = 0;

    virtual ConfigBuilder& options(const QString& options) = 0;

    ConfigBuilder& setDatabaseUpgrader(DatabaseUpgrader* databaseUpgrader);

    virtual QSqlDatabase getConnection(const QString& connectionName, const QString& databaseName) = 0;

    QSqlDatabase getConnection(const QString& connectionName);

    ConfigBuilder& disableCreateDatabase() {
        createDbEnabled = false;
        return *this;
    }

    ConfigBuilder&  disableCreateTables() {
        createTableEnabled = false;
        return *this;
    }

    virtual void initializeDatabase() = 0;

    virtual ~ConfigBuilder() = default;

    bool isSqlite() const {
        return configType == ConfigType::Sqlite;
    }

    bool isMysql() const {
        return configType == ConfigType::Mysql;
    }

    bool isSqlServer() const {
        return configType == ConfigType::SqlServer;
    }

    bool isPSql() const {
        return configType == ConfigType::PSql;
    }

    QSharedPointer<AbstractClient> getClient() const {
        return dbClient;
    }

    static int getLocalVersion(qint64 sessionId = -1);

protected:
    ConfigType configType;

    int mVersion = 1;
    QString mDriver;
    QString mDatabaseName;
    QString mPassword;
    QString mOptions;
    QString mAlias;
    qint64 mSessionId = -1;

    bool createDbEnabled = true;

    bool createTableEnabled = true;

    QSharedPointer<AbstractClient> dbClient;
    QSharedPointer<DatabaseUpgrader> dbUpgrader;

    friend class AbstractClient;
    friend class BaseQuery;
    friend class ::ConnectionPool;
    friend class VersionControl;
    friend class ConfigManager;

protected:
    void setupDatabase();
};

QTDAO_END_NAMESPACE
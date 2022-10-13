#pragma once

#include <qobject.h>

#include "../global.h"
#include "../dbclients/abstractclient.h"

#include "configtype.h"

#include <qsqldatabase.h>
#include <qsharedpointer.h>

QTDAO_BEGIN_NAMESPACE

class ConfigBuilder {
public:
    explicit ConfigBuilder(ConfigType type);

    virtual ConfigBuilder& version(int ver) = 0;

    virtual ConfigBuilder& driver(const QString& driver) = 0;

    virtual ConfigBuilder& databaseName(const QString& name) = 0;

    virtual ConfigBuilder& password(const QString& password) = 0;

    virtual ConfigBuilder& options(const QString& options) = 0;

    virtual QSqlDatabase getConnection(const QString& connectionName, const QString& databaseName) = 0;

    void disableCreateDatabase() {
        createDbEnabled = false;
    }

    void disableCreateTables() {
        createTableEnabled = false;
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

    QSharedPointer<AbstractClient> getClient() const {
        return dbInitClient;
    }

protected:
    ConfigType configType;

    int mVersion = 1;
    QString mDriver;
    QString mDatabaseName;
    QString mPassword;
    QString mOptions;

    bool createDbEnabled = true;

    bool createTableEnabled = true;

    QSharedPointer<AbstractClient> dbInitClient;

    friend class SqliteClient;
    friend class MysqlClient;
    friend class SqlServerClient;
    friend class ConnectionPool;

protected:
    void setupDatabase();

    int getLocalVersion();

    void updateLocalVersion();

    void invokeCreateTables();

    void invokeTableUpgrade();

    QByteArray getDelegateStr();
};

QTDAO_END_NAMESPACE
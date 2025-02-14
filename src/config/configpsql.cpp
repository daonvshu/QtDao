#include "config/configpsql.h"
#include "config/configmanager.h"

#include "dao.h"

QTDAO_BEGIN_NAMESPACE
ConfigPSqlBuilder::ConfigPSqlBuilder()
    : ConfigBuilder(ConfigType::PSql)
    , mHost("localhost")
    , mUser("postgres")
    , mPort(5432)
{}

ConfigPSqlBuilder & ConfigPSqlBuilder::version(int ver) {
    mVersion = ver;
    return *this;
}

ConfigPSqlBuilder & ConfigPSqlBuilder::driver(const QString &driver) {
    mDriver = driver;
    return *this;
}

ConfigPSqlBuilder & ConfigPSqlBuilder::databaseName(const QString &name) {
    mDatabaseName = name;
    return *this;
}

ConfigBuilder & ConfigPSqlBuilder::configAlias(const QString &alias) {
    mAlias = alias;
    return *this;
}

ConfigBuilder & ConfigPSqlBuilder::session(qint64 sessionId) {
    mSessionId = sessionId;
    return *this;
}

ConfigPSqlBuilder & ConfigPSqlBuilder::password(const QString &password) {
    mPassword = password;
    return *this;
}

ConfigPSqlBuilder & ConfigPSqlBuilder::options(const QString &options) {
    mOptions = options;
    return *this;
}

ConfigPSqlBuilder & ConfigPSqlBuilder::host(const QString &host) {
    mHost = host;
    return *this;
}

ConfigPSqlBuilder & ConfigPSqlBuilder::user(const QString &user) {
    mUser = user;
    return *this;
}

ConfigPSqlBuilder & ConfigPSqlBuilder::port(int port) {
    mPort = port;
    return *this;
}

QSqlDatabase ConfigPSqlBuilder::getConnection(const QString &connectionName, const QString &databaseName) {
    auto db = QSqlDatabase::addDatabase(mDriver, connectionName);
    db.setDatabaseName(databaseName);
    db.setUserName(mUser);
    db.setPassword(mPassword);
    db.setHostName(mHost);
    db.setPort(mPort);

    if (!mOptions.isEmpty()) {
        db.setConnectOptions(mOptions);
    }

    return db;
}

void ConfigPSqlBuilder::initializeDatabase() {
    auto config = new ConfigPSqlBuilder(*this);
    ConfigManager::registerConfig(config);
    config->setupDatabase();
}


QTDAO_END_NAMESPACE
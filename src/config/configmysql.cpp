#include "config/configmysql.h"

#include "dao.h"

QTDAO_BEGIN_NAMESPACE
ConfigMysqlBuilder::ConfigMysqlBuilder()
    : ConfigBuilder(ConfigType::Mysql)
    , mHost("localhost")
    , mUser("root")
    , mPort(3306)
{
    
}

ConfigMysqlBuilder& ConfigMysqlBuilder::version(int ver) {
    mVersion = ver;
    return *this;
}

ConfigMysqlBuilder& ConfigMysqlBuilder::driver(const QString& driver) {
    mDriver = driver;
    return *this;
}

ConfigMysqlBuilder& ConfigMysqlBuilder::databaseName(const QString& name) {
    mDatabaseName = name;
    return *this;
}

ConfigMysqlBuilder& ConfigMysqlBuilder::password(const QString& password) {
    mPassword = password;
    return *this;
}

ConfigMysqlBuilder& ConfigMysqlBuilder::options(const QString& options) {
    mOptions = options;
    return *this;
}

ConfigMysqlBuilder& ConfigMysqlBuilder::host(const QString& host) {
    mHost = host;
    return *this;
}

ConfigMysqlBuilder& ConfigMysqlBuilder::user(const QString& user) {
    mUser = user;
    return *this;
}

ConfigMysqlBuilder& ConfigMysqlBuilder::port(int port) {
    mPort = port;
    return *this;
}

QSqlDatabase ConfigMysqlBuilder::getConnection(const QString& connectionName, const QString& databaseName) {
    auto dbName = databaseName;
    if (dbName.isEmpty()) {
        dbName = mDatabaseName;
    }

    auto db = QSqlDatabase::addDatabase(mDriver, connectionName);
    db.setDatabaseName(dbName);
    db.setUserName(mUser);
    db.setPassword(mPassword);
    db.setHostName(mHost);
    db.setPort(mPort);

    if (!mOptions.isEmpty()) {
        db.setConnectOptions(mOptions);
    }

    return db;
}

void ConfigMysqlBuilder::initializeDatabase() {
    globalConfig.reset(new ConfigMysqlBuilder(*this));
    setupDatabase();
}

QTDAO_END_NAMESPACE

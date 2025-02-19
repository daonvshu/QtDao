#include "config/configmysql.h"
#include "config/configmanager.h"

#include "dao.h"

QTDAO_BEGIN_NAMESPACE
ConfigMysqlBuilder::ConfigMysqlBuilder()
    : ConfigBuilder(ConfigType::Mysql)
    , mHost("localhost")
    , mUser("root")
    , mPort(3306)
    , mCharacter("utf8mb4")
    , mCollate("utf8mb4_general_ci")
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

ConfigBuilder &ConfigMysqlBuilder::configAlias(const QString &alias) {
    mAlias = alias;
    return *this;
}

ConfigBuilder &ConfigMysqlBuilder::session(qint64 sessionId) {
    mSessionId = sessionId;
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

ConfigMysqlBuilder & ConfigMysqlBuilder::character(const QString &character) {
    mCharacter = character;
    return *this;
}

ConfigMysqlBuilder & ConfigMysqlBuilder::collate(const QString &collate) {
    mCollate = collate;
    return *this;
}

QSqlDatabase ConfigMysqlBuilder::getConnection(const QString& connectionName, const QString& databaseName) {
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

void ConfigMysqlBuilder::initializeDatabase() {
    auto config = new ConfigMysqlBuilder(*this);
    ConfigManager::registerConfig(config);
    config->setupDatabase();
}

QTDAO_END_NAMESPACE

#include "config/configsqlserver.h"
#include "config/configmanager.h"

QTDAO_BEGIN_NAMESPACE
ConfigSqlServerBuilder::ConfigSqlServerBuilder()
    : ConfigBuilder(ConfigType::SqlServer)
    , mHost("localhost")
    , mUser("sa")
    , mPort(1433)
{
    
}

ConfigSqlServerBuilder& ConfigSqlServerBuilder::version(int ver) {
    mVersion = ver;
    return *this;
}

ConfigSqlServerBuilder& ConfigSqlServerBuilder::driver(const QString& driver) {
    mDriver = driver;
    return *this;
}

ConfigSqlServerBuilder& ConfigSqlServerBuilder::databaseName(const QString& name) {
    mDatabaseName = name;
    return *this;
}

ConfigBuilder &ConfigSqlServerBuilder::configAlias(const QString &alias) {
    mAlias = alias;
    return *this;
}

ConfigBuilder &ConfigSqlServerBuilder::session(qint64 sessionId) {
    mSessionId = sessionId;
    return *this;
}

ConfigSqlServerBuilder& ConfigSqlServerBuilder::password(const QString& password) {
    mPassword = password;
    return *this;
}

ConfigSqlServerBuilder& ConfigSqlServerBuilder::options(const QString& options) {
    mOptions = options;
    return *this;
}

ConfigSqlServerBuilder& ConfigSqlServerBuilder::host(const QString& host) {
    mHost = host;
    return *this;
}

ConfigSqlServerBuilder& ConfigSqlServerBuilder::user(const QString& user) {
    mUser = user;
    return *this;
}

ConfigSqlServerBuilder& ConfigSqlServerBuilder::port(int port) {
    mPort = port;
    return *this;
}

QSqlDatabase ConfigSqlServerBuilder::getConnection(const QString& connectionName, const QString& databaseName) {
    auto db = QSqlDatabase::addDatabase(mDriver, connectionName);
    db.setDatabaseName(QString("DRIVER={SQL SERVER};"
        "SERVER=%1,%2;"
        "DATABASE=%3;"
        "UID=%4;"
        "PWD=%5;"
    )
        .arg(mHost)
        .arg(mPort)
        .arg(databaseName, mUser, mPassword)
    );

    if (!mOptions.isEmpty()) {
        db.setConnectOptions(mOptions);
    }

    return db;
}

void ConfigSqlServerBuilder::initializeDatabase() {
    auto config = new ConfigSqlServerBuilder(*this);
    ConfigManager::registerConfig(config);
    config->setupDatabase();
}

QTDAO_END_NAMESPACE

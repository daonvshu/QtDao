#include "config/configsqlite.h"

#include <qstandardpaths.h>

#include "dao.h"

QTDAO_BEGIN_NAMESPACE
    ConfigSqliteBuilder::ConfigSqliteBuilder()
    : ConfigBuilder(ConfigType::Sqlite)
{
    
}

ConfigSqliteBuilder& ConfigSqliteBuilder::version(int ver) {
    mVersion = ver;
    return *this;
}

ConfigSqliteBuilder& ConfigSqliteBuilder::driver(const QString& driver) {
    mDriver = driver;
    return *this;
}

ConfigSqliteBuilder& ConfigSqliteBuilder::databaseName(const QString& name) {
    mDatabaseName = name;
    return *this;
}

ConfigSqliteBuilder& ConfigSqliteBuilder::password(const QString& password) {
    mPassword = password;
    return *this;
}

ConfigSqliteBuilder& ConfigSqliteBuilder::options(const QString& options) {
    mOptions = options;
    return *this;
}

ConfigSqliteBuilder& ConfigSqliteBuilder::saveDir(const QString& path) {
    mSaveDir = path;
    return *this;
}

QSqlDatabase ConfigSqliteBuilder::getConnection(const QString& connectionName, const QString&) {
    auto db = QSqlDatabase::addDatabase(mDriver, connectionName);
    db.setDatabaseName(getDbStorePath());

    if (!mPassword.isEmpty()) {
        db.setPassword(mPassword);
    }

    if (!mOptions.isEmpty()) {
        db.setConnectOptions(mOptions);
    }

    return db;
}

QString ConfigSqliteBuilder::getDbStoreDirectory() const {
    auto savePath = mSaveDir;
    if (savePath.isEmpty()) {
        savePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    }
    return savePath;
}

QString ConfigSqliteBuilder::getDbStorePath() const {
    return getDbStoreDirectory() + "/" + mDatabaseName + ".db";
}

void ConfigSqliteBuilder::initializeDatabase() {
    globalConfig.reset(new ConfigSqliteBuilder(*this));
    setupDatabase();
}

QTDAO_END_NAMESPACE

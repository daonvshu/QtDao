#include "config/configsqlite.h"
#include "config/configmanager.h"

#include <qstandardpaths.h>
#include <qsqlquery.h>

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

ConfigBuilder &ConfigSqliteBuilder::configAlias(const QString &alias) {
    mAlias = alias;
    return *this;
}

ConfigBuilder &ConfigSqliteBuilder::session(qint64 sessionId) {
    mSessionId = sessionId;
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

    if (db.open()) {
        QSqlQuery query(db);
        query.exec("PRAGMA foreign_keys = ON");
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
    auto config = new ConfigSqliteBuilder(*this);
    ConfigManager::registerConfig(config);
    config->setupDatabase();
}

QTDAO_END_NAMESPACE

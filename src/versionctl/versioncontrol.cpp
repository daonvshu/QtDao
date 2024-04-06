#include "versionctl/versioncontrol.h"
#include "versionctl/version.h"

#include "config/configbuilder.h"
#include "config/configmanager.h"

#include "dbexception.h"
#include "dao.h"

QTDAO_BEGIN_NAMESPACE

void VersionControl::checkLocalVersion(qint64 sessionId) {

    auto config = ConfigManager::getConfig(sessionId);
    auto versionTbReader = new SqliteEntityReaderProvider<Version>; //same suitable for mysql/sqlserver
    config->dbClient->createTableIfNotExist(versionTbReader);
    delete versionTbReader;

    int localVersion = getLocalVersion(sessionId);
    if (localVersion > config->mVersion) {
        throw DaoException("The config version is smaller than the local version!");
    }

    if (localVersion == config->mVersion) {
        return;
    }

    invokeCreateTables(sessionId);
    if (localVersion != -1) {
        invokeTableUpgrade(localVersion, config->mVersion, sessionId);
    }
    updateLocalVersion(sessionId);
}

int VersionControl::getLocalVersion(qint64 sessionId) {
    auto d = dao::_select<Version>(sessionId).build().one();
    return d.getVer();
}

void VersionControl::updateLocalVersion(qint64 sessionId) {

    dao::_delete<Version>(sessionId).build().deleteBy();

    auto config = ConfigManager::getConfig(sessionId);
    auto v = Version(config->mVersion);
    dao::_insert<Version>(sessionId).build().insert(v);
}

QString getAliasKeyPrefix(ConfigType configType) {
    switch (configType) {
        case ConfigType::Sqlite:
            return "sqlite_";
        case ConfigType::Mysql:
            return "mysql_";
        case ConfigType::SqlServer:
            return "sqlserver_";
    }
    return {};
}

void VersionControl::invokeCreateTables(qint64 sessionId) {
    auto config = ConfigManager::getConfig(sessionId);
    ConfigSelector::getConfig(getAliasKeyPrefix(config->configType) + config->mAlias)->createEntityTables(config.data());
}

void VersionControl::invokeTableUpgrade(int oldVer, int newVer, qint64 sessionId) {
    auto config = ConfigManager::getConfig(sessionId);
    ConfigSelector::getConfig(getAliasKeyPrefix(config->configType) + config->mAlias)->entityTablesUpgrade(config.data(), oldVer, newVer);
}

QTDAO_END_NAMESPACE
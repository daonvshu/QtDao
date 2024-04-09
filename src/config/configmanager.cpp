#include "config/configmanager.h"
#include "config/configbuilder.h"

#include "dbexception.h"

#include "connectionpool.h"

QTDAO_BEGIN_NAMESPACE

static QHash<qint64, QSharedPointer<ConfigBuilder>> configs;
static QMutex configAccessLocker;

void ConfigManager::registerConfig(ConfigBuilder *config) {
    QMutexLocker locker(&configAccessLocker);
    if (configs.contains(config->mSessionId)) {
        ConnectionPool::closeConnection(config->mSessionId);
    }
    configs.insert(config->mSessionId, QSharedPointer<ConfigBuilder>(config));
}

QSharedPointer<ConfigBuilder> ConfigManager::getConfig(qint64 sessionId) {
    QMutexLocker locker(&configAccessLocker);
    if (!configs.contains(sessionId)) {
        sessionId = ConnectionPool::scopeCurrentSessionId();
        if (!configs.contains(sessionId)) {
            throw DaoException("session is not register:" + QString::number(sessionId));
        }
    }
    return configs[sessionId];
}

QTDAO_END_NAMESPACE
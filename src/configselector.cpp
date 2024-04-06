#include "configselector.h"
#include "dbexception.h"

#include <qhash.h>

QTDAO_BEGIN_NAMESPACE

typedef QHash<QString, QSharedPointer<EntityConfigDelegate>> TypeEntityConfig;

TypeEntityConfig& getEntityConfigs() {
    static TypeEntityConfig entityConfigs;
    return entityConfigs;
}

int ConfigSelector::registerConfig(const QString& alias, EntityConfigDelegate *delegate) {
    getEntityConfigs().insert(alias, QSharedPointer<EntityConfigDelegate>(delegate));
    return 0;
}

QSharedPointer<EntityConfigDelegate> ConfigSelector::getConfig(const QString &alias) {
    if (!getEntityConfigs().contains(alias)) {
        if (alias.isEmpty()) {
            throw DaoException("cannot find default config to setup database!");
        } else {
            throw DaoException("cannot find config: '" + alias + "' to setup database!");
        }
    }
    return getEntityConfigs()[alias];
}

QTDAO_END_NAMESPACE
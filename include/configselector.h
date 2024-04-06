#pragma once

#include "global.h"

#include <qobject.h>
#include <qsharedpointer.h>

QTDAO_BEGIN_NAMESPACE

class ConfigBuilder;
class EntityConfigDelegate {
public:
    virtual void createEntityTables(ConfigBuilder* config) = 0;
    virtual void entityTablesUpgrade(ConfigBuilder* config, int oldVer, int newVer) = 0;

    virtual ~EntityConfigDelegate() = default;
};

class ConfigSelector {
public:
    static int registerConfig(const QString& alias, EntityConfigDelegate* delegate);

    static QSharedPointer<EntityConfigDelegate> getConfig(const QString& alias);
};

QTDAO_END_NAMESPACE
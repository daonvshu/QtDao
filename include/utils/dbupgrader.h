#pragma once

#include "../global.h"
#include "../query/reader/entityreaderinterface.h"
#include "../config/configtype.h"

QTDAO_BEGIN_NAMESPACE

class DatabaseUpgrader {
public:
    DatabaseUpgrader();

    void setEntityReader(EntityReaderInterface* reader);

    void setConfigType(ConfigType type);

    virtual void onUpgrade(int oldVersion, int curVersion);

protected:
    EntityReaderInterface* entityReader;
    ConfigType currentDatabaseType;
};

QTDAO_END_NAMESPACE
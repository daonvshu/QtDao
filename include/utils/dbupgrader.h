#pragma once

#include "../global.h"
#include "../query/reader/entityreaderinterface.h"

#include <qsharedpointer.h>

QTDAO_BEGIN_NAMESPACE

class AbstractClient;
class ConfigBuilder;

class DatabaseUpgrader {
public:
    DatabaseUpgrader();

    void setEntityReader(EntityReaderInterface* reader);

    void setCurClient(AbstractClient* curClient);

    void setCurConfig(ConfigBuilder* curConfig);

    virtual void onUpgrade(int oldVersion, int curVersion);

protected:
    void upgradeWithDataRecovery();

protected:
    EntityReaderInterface* entityReader;

    AbstractClient* client = nullptr;
    ConfigBuilder* config = nullptr;
};

QTDAO_END_NAMESPACE
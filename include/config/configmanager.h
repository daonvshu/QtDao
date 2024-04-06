#pragma once

#include <qobject.h>
#include <qhash.h>
#include <qmutex.h>

#include "../global.h"

QTDAO_BEGIN_NAMESPACE

class ConfigBuilder;
class ConfigManager {
public:
    static void registerConfig(ConfigBuilder* config);
    static QSharedPointer<ConfigBuilder> getConfig(qint64 sessionId = -1);
};

QTDAO_END_NAMESPACE
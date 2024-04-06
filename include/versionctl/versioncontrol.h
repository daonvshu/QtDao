#pragma once

#include "../global.h"

#include <qobject.h>

QTDAO_BEGIN_NAMESPACE

class VersionControl {
public:
    static void checkLocalVersion(qint64 sessionId);

    static int getLocalVersion(qint64 sessionId);

private:
    static void updateLocalVersion(qint64 sessionId);

    static void invokeCreateTables(qint64 sessionId);

    static void invokeTableUpgrade(int oldVer, int newVer, qint64 sessionId);
};

QTDAO_END_NAMESPACE
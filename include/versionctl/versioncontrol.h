#pragma once

#include "../global.h"

#include <qobject.h>

QTDAO_BEGIN_NAMESPACE

class VersionControl {
public:
    static void checkLocalVersion();

    static int getLocalVersion();

private:
    static void updateLocalVersion();

    static void invokeCreateTables();

    static void invokeTableUpgrade(int oldVer, int newVer);

    static QByteArray getDelegateStr();
};

QTDAO_END_NAMESPACE
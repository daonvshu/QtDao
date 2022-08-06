#pragma once

#include "../global.h"

#include <qobject.h>
#include <qmutex.h>
#include <qwaitcondition.h>
#include <qhash.h>

QTDAO_BEGIN_NAMESPACE

class SqliteLockControl {

public:
    SqliteLockControl();

    void enableSqliteWriteSync(bool enable);

    void trancationStart();
    void trancationPrepareEnd();
    void trancationEnd();

    void testWrite();
    void testRead();
    void releaseWrite();
    void releaseRead();

private:
    bool writeSync; //open write sync
    QMutex writeSyncChecker;

    struct {
        bool globalLock;
        QWaitCondition globalWait;
        bool writeLock;
        QWaitCondition writeWait;
        bool readLock;
        QWaitCondition readWait;
        bool beginReserved;
        QWaitCondition beginWait;
        Qt::HANDLE beginThreadId;
    } lockCondition;

    QMutex conditionChecker;
};

QTDAO_END_NAMESPACE
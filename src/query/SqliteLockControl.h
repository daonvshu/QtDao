#pragma once

#include <qobject.h>
#include <qmutex.h>
#include <qhash.h>

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

    QMutex globalWriteLocker; //global write locker
    QHash<Qt::HANDLE, bool> currentIsTrancation; //current work thread is trancation
    QMutex startTrancationLocker;
    QMutex trancationWriteLocker; //trancation write locker
    QMutex trancationReadLocker; //trancation read locker
};
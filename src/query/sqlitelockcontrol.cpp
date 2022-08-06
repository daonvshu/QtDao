#include "query/sqlitelockcontrol.h"

#include "dbloader.h"

#include <qthread.h>

#include <iostream>

QTDAO_BEGIN_NAMESPACE

SqliteLockControl::SqliteLockControl()
    : writeSync(false)
{
    //clear lock state
    lockCondition.globalLock = false;
    lockCondition.writeLock = false;
    lockCondition.readLock = false;
    lockCondition.beginReserved = false;
}

void print(const char* msg) {
    Q_UNUSED(msg);
    //std::cout << msg << "  thread:" << QThread::currentThreadId() << std::endl;
}

#define WRITE_SYNC_BEGIN \
if (writeSync) { \
    { \
        QMutexLocker syncLock(&writeSyncChecker); \
        if (!writeSync) { \
            return; \
        } \
    }

#define WRITE_SYNC_END }

void SqliteLockControl::enableSqliteWriteSync(bool enable) {
    if (!DbLoader::getConfig().isSqlite()) {
        return;
    }
    print("#1-0");
    QMutexLocker locker(&conditionChecker);
    if (!enable) {
        lockCondition.globalWait.wakeAll();
        lockCondition.writeWait.wakeAll();
        lockCondition.readWait.wakeAll();
        lockCondition.beginWait.wakeAll();
    }
    writeSync = enable;
    print("#1-4");
}

void SqliteLockControl::trancationStart() {
    WRITE_SYNC_BEGIN
    print("#2-1");
    conditionChecker.lock();
    if (lockCondition.beginReserved) {
        print("#2-2");
        lockCondition.beginWait.wait(&conditionChecker);
    }
    print("#2-3");
    lockCondition.beginReserved = true;
    lockCondition.beginThreadId = QThread::currentThreadId();

    if (lockCondition.writeLock) {
        print("#2-4");
        lockCondition.writeWait.wait(&conditionChecker);
    }
    print("#2-5");
    lockCondition.writeLock = true;

    conditionChecker.unlock();
    WRITE_SYNC_END
}

void SqliteLockControl::trancationPrepareEnd() {
    WRITE_SYNC_BEGIN
    print("#3-0");
    conditionChecker.lock();
    if (lockCondition.readLock) {
        print("#3-1");
        lockCondition.readWait.wait(&conditionChecker);
    }
    print("#3-2");
    lockCondition.readLock = true;
    conditionChecker.unlock();
    WRITE_SYNC_END
}

void SqliteLockControl::trancationEnd() {
    WRITE_SYNC_BEGIN
    print("#4-2");
    conditionChecker.lock();

    lockCondition.beginReserved = false;
    lockCondition.beginWait.wakeOne();

    lockCondition.readLock = false;
    lockCondition.readWait.wakeAll();

    lockCondition.writeLock = false;
    lockCondition.writeWait.wakeAll();

    conditionChecker.unlock();
    print("#4-3");
    WRITE_SYNC_END
}

void SqliteLockControl::testWrite() {
    WRITE_SYNC_BEGIN
    print("#5-2");
    conditionChecker.lock();
    if (lockCondition.beginReserved) {
        if (lockCondition.beginThreadId == QThread::currentThreadId()) {
            conditionChecker.unlock();
            print("#5-3");
            return;
        }
    }
    print("#5-4");
    if (lockCondition.globalLock) {
        lockCondition.globalWait.wait(&conditionChecker);
    }
    lockCondition.globalLock = true;
    print("#5-5");
    if (lockCondition.writeLock) {
        lockCondition.writeWait.wait(&conditionChecker);
    }
    lockCondition.writeLock = true;
    print("#5-6");
    conditionChecker.unlock();
    WRITE_SYNC_END
}

void SqliteLockControl::testRead() {
    WRITE_SYNC_BEGIN
    print("#6-1");
    conditionChecker.lock();
    if (lockCondition.beginReserved) {
        if (lockCondition.beginThreadId == QThread::currentThreadId()) {
            conditionChecker.unlock();
            print("#6-2");
            return;
        }
    }
    if (lockCondition.globalLock) {
        print("#6-3");
        lockCondition.globalWait.wait(&conditionChecker);
    }
    lockCondition.globalLock = true;
    print("#6-4");
    if (lockCondition.readLock) {
        print("#6-5");
        lockCondition.readWait.wait(&conditionChecker);
    }
    print("#6-6");
    lockCondition.readLock = true;
    conditionChecker.unlock();
    WRITE_SYNC_END
}

void SqliteLockControl::releaseWrite() {
    WRITE_SYNC_BEGIN
    print("#7-1");
    conditionChecker.lock();
    if (lockCondition.beginReserved) {
        if (lockCondition.beginThreadId == QThread::currentThreadId()) {
            conditionChecker.unlock();
            print("#7-2");
            return;
        }
    }
    print("#7-3");
    lockCondition.writeLock = false;
    lockCondition.writeWait.wakeOne();

    lockCondition.globalLock = false;
    lockCondition.globalWait.wakeOne();

    conditionChecker.unlock();
    WRITE_SYNC_END
}

void SqliteLockControl::releaseRead() {
    WRITE_SYNC_BEGIN
    conditionChecker.lock();
    print("#8-1");
    if (lockCondition.beginReserved) {
        if (lockCondition.beginThreadId == QThread::currentThreadId()) {
            conditionChecker.unlock();
            print("#8-2");
            return;
        }
    }
    lockCondition.readLock = false;
    lockCondition.readWait.wakeOne();

    lockCondition.globalLock = false;
    lockCondition.globalWait.wakeOne();

    conditionChecker.unlock();
    print("#8-3");
    WRITE_SYNC_END
}

QTDAO_END_NAMESPACE
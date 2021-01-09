#include "SqliteLockControl.h"

#include "../DbLoader.h"

#include <qthread.h>

#include <iostream>

SqliteLockControl::SqliteLockControl()
    : writeSync(false)
{
}

void print(const char* msg) {
    std::cout << msg << "  thread:" << QThread::currentThreadId() << std::endl;
}

void SqliteLockControl::enableSqliteWriteSync(bool enable) {
    if (!DbLoader::getConfig().isSqlite()) {
        return;
    }
    print("#1-0");
    QMutexLocker locker(&writeSyncChecker);
    if (!enable) {
        print("#1-1");
        QMutexLocker locker1(&globalWriteLocker);
        print("#1-2");
        QMutexLocker locker2(&trancationWriteLocker);
        print("#1-3");
        QMutexLocker locker3(&trancationReadLocker);
        writeSync = enable;
    }
    writeSync = enable;
    print("#1-4");
}

void SqliteLockControl::trancationStart() {
    print("#2-0");
    if (writeSync) {
        {
            QMutexLocker locker1(&writeSyncChecker);
            if (!writeSync) {
                return;
            }
        }
        QMutexLocker lock(&startTrancationLocker);
        currentIsTrancation.insert(QThread::currentThreadId(), true);
    }
    print("#2-1");
}

void SqliteLockControl::trancationPrepareEnd() {
    if (writeSync) {
        {
            QMutexLocker locker1(&writeSyncChecker);
            if (!writeSync) {
                return;
            }
        }
        print("#3-0");
        {
            QMutexLocker lock(&startTrancationLocker);
        }
        currentIsTrancation.insert(QThread::currentThreadId(), false);
        print("#3-1");
        trancationReadLocker.lock();
        print("#3-3");
    }
    print("#3-4");
}

void SqliteLockControl::trancationEnd() {
    if (writeSync) {
        {
            print("#4-0");
            QMutexLocker locker1(&writeSyncChecker);
            print("#4-1");
            if (!writeSync) {
                return;
            }
        }
        print("#4-2");
        trancationReadLocker.unlock();
        trancationWriteLocker.unlock();
        print("#4-3");
    }
}

void SqliteLockControl::testWrite() {
    if (writeSync) {
        {
            print("#5-0");
            QMutexLocker locker1(&writeSyncChecker);
            print("#5-1");
            if (!writeSync) {
                return;
            }
        }
        {
            QMutexLocker lock2(&startTrancationLocker);
        }
        print("#5-2");
        bool currentTrancate = currentIsTrancation.value(QThread::currentThreadId(), false);
        if (!currentTrancate) {
            print("#5-3");
            globalWriteLocker.lock();
            print("#5-4");
        }
        trancationWriteLocker.lock();
        print("#5-5");
        print("#5-6");
    }
}

void SqliteLockControl::testRead() {
    if (writeSync) {
        {
            print("#6-0");
            QMutexLocker locker1(&writeSyncChecker);
            print("#6-1");
            if (!writeSync) {
                return;
            }
        }
        print("#6-2");
        {
            QMutexLocker locker2(&globalWriteLocker);
        }
        print("#6-3");
        trancationReadLocker.lock();
        print("#6-4");
    }
}

void SqliteLockControl::releaseWrite() {
    if (writeSync) {
        {
            print("#7-0");
            QMutexLocker locker1(&writeSyncChecker);
            print("#7-1");
            if (!writeSync) {
                return;
            }
        }
        {
            QMutexLocker lock2(&startTrancationLocker);
        }
        print("#7-2");
        bool currentTrancate = currentIsTrancation.value(QThread::currentThreadId(), false);
        if (!currentTrancate) {
            print("#7-3");
            trancationWriteLocker.unlock();
            print("#7-4");
            globalWriteLocker.unlock();
        }
        print("#7-5");
    }
}

void SqliteLockControl::releaseRead() {
    if (writeSync) {
        {
            print("#8-0");
            QMutexLocker locker1(&writeSyncChecker);
            print("#8-1");
            if (!writeSync) {
                return;
            }
        }
        print("#8-2");
        trancationReadLocker.unlock();
        print("#8-3");
    }
}

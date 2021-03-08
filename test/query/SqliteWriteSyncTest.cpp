#include "SqliteWriteSyncTest.h"

#include "../sqliteentity/SqliteTest1.h"
#include "../sqliteentity/SqliteTest2.h"

#include "../RunnableHandler.h"

#include <QtTest>
#include <QMutex>
#include <iostream>
#include <qeventloop.h>
#include <qdatetime.h>

void SqliteWriteSyncTest::initTestCase() {
    configDb();
    clearCache();
}

/*
 * thread lock sync before ################
 * thread1         thread2          state
 * exclusive                        block
 *                 pending          wait
 *                 pending          timeout
 *
 * thread lock sync after ################
 * thread1         thread2          state
 * global wlock                     block rw
 *                 global wlock     wait
 * exclusive                        block
 * unlock
 *                 pending
 *                 unlock
 */
void SqliteWriteSyncTest::testInsertWriteLock() {
    PASSMYSQL;
    PASSSQLSERVER;
    dao::sqlWriteSync(true);
    //allocate 200Mb bytes data
    QByteArray data(200 * 1024 * 1024, 0x01);
    QEventLoop loop;
    QMutex locker;
    bool testQuit = false;
    RunnableHandler<void>::exec([&] {
        SqliteTest1 test(1000L, "test large data insert", 1, data);
        dao::_insert<SqliteTest1>().build().insertOrReplace(test);
        locker.lock();
        if (testQuit) {
            loop.quit();
        } else {
            testQuit = true;
        }
        locker.unlock();
    });

    QByteArray lastErr;
    RunnableHandler<void>::exec([&] {
        //wait thread1 get exclusive locker
        QThread::msleep(200);
        try {
            SqliteTest1::Fields sf;
            dao::_select<SqliteTest1>().throwable().filter(sf.name == "test large data insert").build().one();
        } catch (DaoException& e) {
            lastErr = e.reason.toLocal8Bit();
        }
        locker.lock();
        if (testQuit) {
            loop.quit();
        } else {
            testQuit = true;
        }
        locker.unlock();
    });
    loop.exec();
    QVERIFY2(lastErr.isEmpty(), lastErr);
    dao::sqlWriteSync(false);
}

/*
 * thread lock sync before ################
 * thread1         thread2         thread3        state
 * reserved                        shared
 * exclusive                                      block
 *                                 shared         wait
 *                 trancation
 *                 pending                        wait
 * unlock
 *                                 shared
 *                 reserved
 * pending                                        wait
 * pending                                        timeout
 * 
 * thread lock sync after ################
 * thread1         thread2                        state
 * global wlock                                   block rw
 *                                 global wlock   wait
 * tranc wlock                                    block trancation write
 *                 trancation
 *                 tranc wlock                    wait
 * exclusive                                      block
 * tranc wunlock
 *                 tranc wlock
 * global unlock
 *                                 global wlock   block rw
 *                                 tranc rlock    tranc block
 *                                 shared
 * global wlock                                   wait
 *                 exclusive                      block
 *                 unlock                         tranc block
 *                 tranc rlock                    wait
 *                                 unlock
 *                                 tranc runlock
 *                 tranc rlock
 *                                 tranc rlock    wait
 *                 tranc runlock
 *                                 tranc rlock
 *                                 shared
 *                                 global unlock
 *                 tranc wunlock
 * global wlock
 */
void SqliteWriteSyncTest::testTranscationWriteLock() {
    PASSMYSQL;
    PASSSQLSERVER;
    dao::sqlWriteSync(true);
    qint64 startPoint = QDateTime::currentMSecsSinceEpoch();
    QEventLoop loop;
    SqliteTest2::Fields sf2;
    bool insertExit = false;
    QMutex locker;
    RunnableHandler<void>::exec([&] {
        int number1 = 100000;
        while (true) {
            int diff = QDateTime::currentMSecsSinceEpoch() - startPoint;
            if (diff > 5000) {
                break;
            }
            int diffSec = diff / 1000;
            static int lastDiff = -1;
            if (lastDiff != diffSec) {
                lastDiff = diffSec;
                std::cout << "testing sqlite write lock: " << lastDiff << std::endl;
            }
            try {
                SqliteTest2 data("test1", number1, 0, "aaa");
                /*std::cout << "prepare insert thread1: " << QThread::currentThreadId()
                    << " t -> " << QDateTime::currentMSecsSinceEpoch() << std::endl;*/
                dao::_insert<SqliteTest2>().throwable().build().insert(data);
                number1++;
            }
            catch (DaoException& e) {
                Q_UNUSED(e);
                QMutexLocker l(&locker);
                insertExit = true;
                loop.quit();
                QFAIL(("test sqlite write lock fail #1!" + e.reason).toLocal8Bit());
            }
        }
        loop.quit();
    });
    bool thread2Running = true;
    RunnableHandler<void>::exec([&] {
        int number2 = 1000;
        dao::transcation();
        try {
            while (QDateTime::currentMSecsSinceEpoch() - startPoint < 3000) {
                locker.lock();
                if (insertExit) {
                    thread2Running = false;
                    locker.unlock();
                    return;
                }
                locker.unlock();
                if (number2 % 2) {
                    SqliteTest2 data("test2", number2, 0, "aaa");
                    /*std::cout << "prepare insert thread2: " << QThread::currentThreadId()
                        << " t -> " << QDateTime::currentMSecsSinceEpoch() << std::endl;*/
                    dao::_insert<SqliteTest2>().throwable().build().insert(data);
                }
                number2++;
            }
            std::cout << "prepare commit: " << QThread::currentThreadId() << std::endl;
            dao::commit();
        }
        catch (DaoException& e) {
            dao::rollback();
            QMutexLocker l(&locker);
            thread2Running = false;
            QFAIL(("test sqlite write lock fail #2!" + e.reason).toLocal8Bit());
        }
        QMutexLocker l(&locker);
        thread2Running = false;
        std::cout << "thread2 commit finished!" << std::endl;
    });
    bool thread3Running = true;
    RunnableHandler<void>::exec([&] {
        try {
            while (QDateTime::currentMSecsSinceEpoch() - startPoint < 4000) {
                locker.lock();
                if (insertExit) {
                    thread3Running = false;
                    locker.unlock();
                    return;
                }
                locker.unlock();
                SqliteTest2::Fields sf;
                /*std::cout << "prepare select thread3: " << QThread::currentThreadId()
                    << " t -> " << QDateTime::currentMSecsSinceEpoch() << std::endl;*/
                dao::_select<SqliteTest2>().throwable().filter(sf.name == "test").build().list();
            }
        } catch (DaoException& e) {
            QMutexLocker l(&locker);
            thread3Running = false;
            QFAIL(("test sqlite write lock fail #3!" + e.reason).toLocal8Bit());
        }
        QMutexLocker l(&locker);
        thread3Running = false;
    });
    loop.exec();
    while (true) {
        QMutexLocker l(&locker);
        if (!thread2Running && !thread3Running) break;
    }
    dao::sqlWriteSync(false);
}


void SqliteWriteSyncTest::cleanup() {
    clearCache();
}

void SqliteWriteSyncTest::cleanupTestCase() {
    ConnectionPool::release();
    DbLoader::getClient().dropDatabase();
}

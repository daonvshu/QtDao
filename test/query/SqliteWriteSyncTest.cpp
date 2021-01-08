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

void SqliteWriteSyncTest::testInsertWriteLock() {
    PASSMYSQL;
    PASSSQLSERVER;
    //allocate 200Mb bytes data
    QByteArray data(200 * 1024 * 1024, 0x01);
    QEventLoop loop;
    QMutex locker;
    bool testQuit = false;
    RunnableHandler<void>::exec([&] {
        qint64 startT = QDateTime::currentMSecsSinceEpoch();
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
        QThread::msleep(100);
        try {
            SqliteTest1::Fields sf;
            dao::_select<SqliteTest1>().throwable().filter(sf.name == "test large data insert").build().one();
        } catch (DaoException& e) {
            lastErr = e.reason.toUtf8();
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
}

void SqliteWriteSyncTest::testTranscationWriteLock() {
    PASSMYSQL;
    PASSSQLSERVER;
    qint64 startPoint = QDateTime::currentMSecsSinceEpoch();
    QEventLoop loop;
    SqliteTest2::Fields sf2;
    bool insertExit = false;
    QMutex locker;
    dao::sqlWriteSync(true);
    RunnableHandler<void>::exec([&] {
        int number1 = 10000;
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
                if (number1 % 2) {
                    SqliteTest2 data("test", number1, 0, "aaa");
                    std::cout << "prepare insert: " << QThread::currentThreadId() << std::endl;
                    dao::_insert<SqliteTest2>().throwable().build().insert(data);
                }
                number1++;
            }
            catch (DaoException& e) {
                Q_UNUSED(e);
                QMutexLocker l(&locker);
                insertExit = true;
                loop.quit();
                QFAIL(("test sqlite write lock fail #1!" + e.reason).toUtf8());
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
                    SqliteTest2 data("test", number2, 0, "aaa");
                    std::cout << "prepare insert: " << QThread::currentThreadId() << std::endl;
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
            QFAIL(("test sqlite write lock fail #2!" + e.reason).toUtf8());
        }
        QMutexLocker l(&locker);
        thread2Running = false;
    });
    loop.exec();
    while (true) {
        QMutexLocker l(&locker);
        if (!thread2Running) break;
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

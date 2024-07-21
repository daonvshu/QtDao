#include "ConnectionPoolTest.h"

#include "connectionpool.h"

#include "dbclients/sqliteclient.h"
#include "dao.h"

#include <qeventloop.h>
#include <QtConcurrent/QtConcurrent>
#include <QThread>
#include <qatomic.h>

#include <QtTest>

QTDAO_USING_NAMESPACE

void ConnectionPoolTest::initTestCase() {
    setupDatabase("corelib_test");
    QThreadPool::globalInstance()->setMaxThreadCount(10);
}

void ConnectionPoolTest::testConnect() {
    auto db = ConnectionPool::getConnection();
    QVERIFY(db.isOpen());
    QSqlQuery query(db);
    query.exec("select 1");
}

void ConnectionPoolTest::testReuseConnection() {
    QString savedConnection;
    auto db = ConnectionPool::getConnection();
    QVERIFY(db.isOpen());
    savedConnection = db.connectionName();

    db = ConnectionPool::getConnection();
    QVERIFY(db.isOpen());
    QCOMPARE(db.connectionName(), savedConnection);
}

void ConnectionPoolTest::testSerialThread() {
    QEventLoop loop;
    QString connection1, connection2;
    QThread::create([&] {
        auto db = ConnectionPool::getConnection();
        QVERIFY(db.isOpen());
        connection1 = db.connectionName();
        //close connection when current work thread over
        loop.quit();
    })->start();

    loop.exec(); //wait thread exit
    QThread::msleep(100); //wait connection recycle

    QThread::create([&] {
        auto db = ConnectionPool::getConnection();
        QVERIFY(db.isOpen());
        connection2 = db.connectionName();
        loop.quit();
    })->start();

    loop.exec(); //wait thread exit

    QCOMPARE(connection1, connection2);
}

void ConnectionPoolTest::testParallelThread() {
    QString connection1, connection2;
    QAtomicInt threadCount = 0;

    QThread::create([&] {
        threadCount++;
        auto db = ConnectionPool::getConnection();
        QVERIFY(db.isOpen());
        connection1 = db.connectionName();
        QThread::msleep(100);
        threadCount--;
    })->start();

    QThread::create([&] {
        threadCount++;
        auto db = ConnectionPool::getConnection();
        QVERIFY(db.isOpen());
        connection2 = db.connectionName();
        threadCount--;
    })->start();

    QThread::msleep(100); //wait for start
    while (threadCount.loadAcquire() != 0) QThread::msleep(100);
    QVERIFY(connection1 != connection2);
}

void ConnectionPoolTest::testSerialThreadPool() {
    QString connection1, connection2;
    Qt::HANDLE thread1, thread2;
    QEventLoop loop;

    QtConcurrent::run([&] {
        thread1 = QThread::currentThreadId();
        auto db = ConnectionPool::getConnection();
        QVERIFY(db.isOpen());
        connection1 = db.connectionName();
        loop.quit();
    });
    loop.exec(); //wait run exit

    QtConcurrent::run([&] {
        thread2 = QThread::currentThreadId();
        auto db = ConnectionPool::getConnection();
        QVERIFY(db.isOpen());
        connection2 = db.connectionName();
        loop.quit();
    });

    loop.exec(); //wait run exit
    QVERIFY((thread1 == thread2 && connection1 == connection2) || (thread1 != thread2 && connection1 != connection2));
}

void ConnectionPoolTest::testParallelThreadPool() {
    QString connection1, connection2;
    QAtomicInt threadCount = 0;

    QtConcurrent::run([&] {
        threadCount++;
        auto db = ConnectionPool::getConnection();
        QVERIFY(db.isOpen());
        connection1 = db.connectionName();
        threadCount--;
    });

    QtConcurrent::run([&] {
        threadCount++;
        auto db = ConnectionPool::getConnection();
        QVERIFY(db.isOpen());
        connection2 = db.connectionName();
        threadCount--;
    });

    QThread::msleep(100); //wait for start
    while (threadCount.loadAcquire() != 0) QThread::msleep(100); //wait run exit
    QVERIFY(connection1 != connection2);
}

void ConnectionPoolTest::testGoneAway() {
    PASSSQLITE;
    PASSSQLSERVER;

    QEventLoop loop;
    BaseQuery::queryPrimitive("set global wait_timeout=1");

    QThread::create([&] {
        {
            auto db = ConnectionPool::getConnection();
            QSqlQuery query(db);
            query.exec("select 1");
        }
        QThread::sleep(3);
        {
            auto db = ConnectionPool::getConnection();
            QSqlQuery query(db);
            query.exec("select 1");
        }
        loop.quit();
    })->start();

    loop.exec(); //wait thread exit

    BaseQuery::queryPrimitive("set global wait_timeout=28800");
}

void ConnectionPoolTest::cleanup() {
    QThread::msleep(50);
}

void ConnectionPoolTest::cleanupTestCase() {
    ConfigManager::getConfig()->getClient()->dropDatabase();
}


#include "ConnectionPoolTest.h"

#include "../../src/DbLoader.h"
#include "../../src/ConnectionPool.h"

#include "../../src/dbclients/SqliteClient.h"
#include "../../src/dao.h"

#include "sqliteentity/SqliteConfig.h"

#include "RunnableHandler.h"

#include <qeventloop.h>

#include <QtTest>

void ConnectionPoolTest::initTestCase() {
    SqliteClient().testConnect();
    QThreadPool::globalInstance()->setMaxThreadCount(10);
}

void ConnectionPoolTest::testSqliteConnect() {
    DbLoader::loadConfig(SqliteConfig());
    {
        auto db = ConnectionPool::getConnection();
        QVERIFY(db.isOpen());
        QSqlQuery query(db);
        query.exec("select 1");
    }
    ConnectionPool::closeConnection();
}

void ConnectionPoolTest::testReuseConnection() {
    DbLoader::loadConfig(SqliteConfig());
    {
        QString savedConnection;
        auto db = ConnectionPool::getConnection();
        QVERIFY(db.isOpen());
        savedConnection = db.connectionName();

        db = ConnectionPool::getConnection();
        QVERIFY(db.isOpen());
        QVERIFY(db.connectionName() == savedConnection);
    }
    ConnectionPool::closeConnection();
    QCOMPARE(ConnectionPool::getUsedConnectionSize(), 1);
}

void ConnectionPoolTest::testMultiThreadOpenConnection() {
    DbLoader::loadConfig(SqliteConfig());
    QEventLoop loop;

    QString connection1, connection2;
    RunnableHandler<void>::exec([&] {
        {
            auto db = ConnectionPool::getConnection();
            QVERIFY(db.isOpen());
            connection1 = db.connectionName();
            QThread::msleep(100);
        }
        ConnectionPool::closeConnection(); //close connection when current work thread over
        loop.quit();
    });

    QThread::msleep(20);

    RunnableHandler<void>::exec([&] {
        {
            auto db = ConnectionPool::getConnection();
            QVERIFY(db.isOpen());
            connection2 = db.connectionName();
        }
        ConnectionPool::closeConnection();
    });
    loop.exec();

    QVERIFY(connection1 != connection2);
    QCOMPARE(ConnectionPool::getUsedConnectionSize(), 2);
}

void ConnectionPoolTest::testReuseConnectionInOtherThread() {
    DbLoader::loadConfig(SqliteConfig());
    QEventLoop loop;

    QString connection1, connection2;

    RunnableHandler<void>::exec([&] {
        {
            auto db = ConnectionPool::getConnection();
            QVERIFY(db.isOpen());
            connection1 = db.connectionName();
            QSqlQuery query(db);
            query.exec("select 1");
        }
        ConnectionPool::closeConnection();
        loop.quit();
        //thread hold
        QThread::msleep(200);
        loop.quit();
    });
    loop.exec();

    //new thread
    RunnableHandler<void>::exec([&] {
        {
            auto db = ConnectionPool::getConnection();
            QSqlQuery query(db);
            query.exec("select 1");
            QVERIFY(db.isOpen());
            connection2 = db.connectionName();
        }
        ConnectionPool::closeConnection();
        QThread::msleep(20);
    });
    loop.exec();

    QVERIFY(connection1 == connection2);
    QCOMPARE(ConnectionPool::getUsedConnectionSize(), 1);
}

void ConnectionPoolTest::testAutoClose() {
    DbLoader::loadConfig(SqliteConfig());
    QEventLoop loop;

    QString connection1, connection2;

    RunnableHandler<void>::exec([&] {
        dao::LocalQuery local;
        {
            auto db = ConnectionPool::getConnection();
            QVERIFY(db.isOpen());
            connection1 = db.connectionName();
            QSqlQuery query(db);
            query.exec("select 1");
        }
        QThread::msleep(200);
        loop.quit();
    });
    loop.exec();

    //new thread
    RunnableHandler<void>::exec([&] {
        dao::LocalQuery local;
        {
            auto db = ConnectionPool::getConnection();
            QVERIFY(db.isOpen());
            QSqlQuery query(db);
            query.exec("select 1");
            connection2 = db.connectionName();
        }
        QThread::msleep(20);
        loop.quit();
    });
    loop.exec();

    QVERIFY(connection1 == connection2);
    QCOMPARE(ConnectionPool::getUsedConnectionSize(), 1);
}

void ConnectionPoolTest::cleanup() {
    ConnectionPool::release();
}

void ConnectionPoolTest::cleanupTestCase() {
    SqliteClient().dropDatabase();
}


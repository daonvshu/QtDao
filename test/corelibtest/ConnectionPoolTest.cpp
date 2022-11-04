#include "ConnectionPoolTest.h"

#include "connectionpool.h"

#include "dbclients/sqliteclient.h"
#include "dao.h"

#include "RunnableHandler.h"

#include <qeventloop.h>

#include <QtTest>

QTDAO_USING_NAMESPACE

void ConnectionPoolTest::initTestCase() {
    setupDatabase();
    ConnectionPool::closeConnection();
    QThreadPool::globalInstance()->setMaxThreadCount(10);
}

void ConnectionPoolTest::testConnect() {
    {
        auto db = ConnectionPool::getConnection();
        QVERIFY(db.isOpen());
        QSqlQuery query(db);
        query.exec("select 1");
    }
    ConnectionPool::closeConnection();
}

void ConnectionPoolTest::testReuseConnection() {
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
    QEventLoop loop;

    QString connection1, connection2;

    RunnableHandler<void>::exec([&] {
        {
            SCOPE_CONNECTION
            {
                auto db = ConnectionPool::getConnection();
                QVERIFY(db.isOpen());
                connection1 = db.connectionName();
                QSqlQuery query(db);
                query.exec("select 1");
            }
        }
        QThread::msleep(200);
        loop.quit();
    });
    loop.exec();

    //new thread
    RunnableHandler<void>::exec([&] {
        {
            SCOPE_CONNECTION
            {
                auto db = ConnectionPool::getConnection();
                QVERIFY(db.isOpen());
                QSqlQuery query(db);
                query.exec("select 1");
                connection2 = db.connectionName();
            }
        }
        QThread::msleep(20);
        loop.quit();
    });
    loop.exec();

    QVERIFY(connection1 == connection2);
    QCOMPARE(ConnectionPool::getUsedConnectionSize(), 1);
}

void ConnectionPoolTest::testGoneAway() {
    PASSSQLITE;
    PASSSQLSERVER;

    QEventLoop loop;

    BaseQuery::queryPrimitive("set global wait_timeout=10");

    RunnableHandler<void>::exec([&] {
        {
            auto db = ConnectionPool::getConnection();
            QSqlQuery query(db);
            query.exec("select 1");
        }
        QThread::sleep(12);
        {
            auto db = ConnectionPool::getConnection();
            QSqlQuery query(db);
            query.exec("select 1");
        }

        loop.quit();
    });
    loop.exec();

    BaseQuery::queryPrimitive("set global wait_timeout=28800");
}

void ConnectionPoolTest::cleanup() {
    QThread::msleep(50);
    ConnectionPool::release();
}

void ConnectionPoolTest::cleanupTestCase() {
    globalConfig->getClient()->dropDatabase();
}


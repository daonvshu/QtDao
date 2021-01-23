#include "ConnectionPoolTest.h"

#include "DbLoader.h"
#include "ConnectionPool.h"

#include "dbclients/SqliteClient.h"
#include "dao.h"

#include "sqliteentity/SqliteConfig.h"
#include "mysqlentity/MysqlConfig.h"

#include "RunnableHandler.h"

#include <qeventloop.h>

#include <QtTest>

void ConnectionPoolTest::initTestCase() {
    if (engineModel == Engine_Sqlite) {
        SqliteClient().testConnect();
    }
    QThreadPool::globalInstance()->setMaxThreadCount(10);
}

void ConnectionPoolTest::testConnect() {
    loadConfigByEngineModel();
    {
        auto db = ConnectionPool::getConnection();
        QVERIFY(db.isOpen());
        QSqlQuery query(db);
        query.exec("select 1");
    }
    ConnectionPool::closeConnection();
}

void ConnectionPoolTest::testReuseConnection() {
    loadConfigByEngineModel();
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
    loadConfigByEngineModel();
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
    loadConfigByEngineModel();
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
    loadConfigByEngineModel();
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
    QThread::msleep(50);
    ConnectionPool::release();
}

void ConnectionPoolTest::cleanupTestCase() {
    DbLoader::getClient().dropDatabase();
}

void ConnectionPoolTest::loadConfigByEngineModel() {
    switch (engineModel) {
    case Engine_Sqlite:
        DbLoader::loadConfig(SqliteConfig());
        break;
    case Engine_Mysql:
        DbLoader::init(MysqlConfig());
        ConnectionPool::release(); //connection pool used by dbload
        break;
    case Engine_SqlServer:
        break;
    }
}


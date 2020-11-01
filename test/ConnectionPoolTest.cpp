#include "ConnectionPoolTest.h"

#include "../src/DbLoader.h"
#include "../src/ConnectionPool.h"

#include "RunnableHandler.h"

#include <qeventloop.h>

#include <QtTest/qtest.h>

void ConnectionPoolTest::initTestCase() {
    auto appLocal = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir;
    if (!dir.exists(appLocal)) {
        if (!dir.mkdir(appLocal)) {
            throw "cannot create sqlite store path!";
        }
    }

    QThreadPool::globalInstance()->setMaxThreadCount(10);
}

void ConnectionPoolTest::testSqliteConnect() {
    DbLoader::loadConfig(":/QtDao/entity/sqlite_cfg.xml");
    auto db = ConnectionPool::openConnection();
    QVERIFY(db.isOpen());
    QSqlQuery query(db);
    query.exec("select 1");
    ConnectionPool::closeConnection(db);
}

void ConnectionPoolTest::testReuseConnection() {
    DbLoader::loadConfig(":/QtDao/entity/sqlite_cfg.xml");

    QString savedConnection;
    auto db = ConnectionPool::openConnection();
    QVERIFY(db.isOpen());
    savedConnection = db.connectionName();
    ConnectionPool::closeConnection(db);

    db = ConnectionPool::openConnection();
    QVERIFY(db.isOpen());
    QVERIFY(db.connectionName() == savedConnection);
    ConnectionPool::closeConnection(db);
}

void ConnectionPoolTest::testMultiThreadOpenConnection() {
    DbLoader::loadConfig(":/QtDao/entity/sqlite_cfg.xml");
    QEventLoop loop;

    QString connection1, connection2;
    RunnableHandler<void>::exec([&] {
        {
            auto db = ConnectionPool::openConnection();
            QVERIFY(db.isOpen());
            connection1 = db.connectionName();
            QThread::msleep(100);
        }
        ConnectionPool::closeConnection(connection1);
        loop.quit();
    });

    QThread::msleep(20);

    RunnableHandler<void>::exec([&] {
        {
            auto db = ConnectionPool::openConnection();
            QVERIFY(db.isOpen());
            connection2 = db.connectionName();
        }
        ConnectionPool::closeConnection(connection2);
    });
    loop.exec();

    QVERIFY(connection1 != connection2);
}

void ConnectionPoolTest::testReuseConnectionInOtherThread() {
    DbLoader::loadConfig(":/QtDao/entity/sqlite_cfg.xml");
    QEventLoop loop;

    QString connection1, connection2;

    RunnableHandler<void>::exec([&] {
        {
            auto db = ConnectionPool::openConnection();
            QVERIFY(db.isOpen());
            connection1 = db.connectionName();
            QSqlQuery query(db);
            query.exec("select 1");
        }
        ConnectionPool::closeConnection(connection1);
        loop.quit();
        //thread hold
        QThread::msleep(200);
        loop.quit();
    });
    loop.exec();

    //new thread
    RunnableHandler<void>::exec([&] {
        {
            auto db = ConnectionPool::openConnection();
            QSqlQuery query(db);
            query.exec("select 1");
            QVERIFY(db.isOpen());
            connection2 = db.connectionName();
        }
        ConnectionPool::closeConnection(connection2);
        QThread::msleep(20);
    });
    loop.exec();

    QVERIFY(connection1 == connection2);
}

void ConnectionPoolTest::cleanup() {
    ConnectionPool::release();
}

void ConnectionPoolTest::cleanupTestCase() {
}

#include "connectionpool.h"
#include <qthread.h>

#include "dbexceptionhandler.h"
#include "dao.h"

#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>

QTDAO_USING_NAMESPACE

#define MAX_CONNECTION_SIZE    200

QMutex ConnectionPool::mutex;

ConnectionPool::~ConnectionPool() {
    release();
}

ConnectionPool& ConnectionPool::getInstance() {
    static ConnectionPool connectionPool;
    return connectionPool;
}

void ConnectionPool::release() {
    // close all connection when destoryed, call release()
    mutex.lock();
    foreach(QString connectionName, getInstance().keepConnections) {
        QSqlDatabase::removeDatabase(connectionName);
    }
    getInstance().keepConnections.clear();

    foreach(QString connectionName, getInstance().unusedConnectionNames) {
        QSqlDatabase::removeDatabase(connectionName);
    }
    getInstance().unusedConnectionNames.clear();
    mutex.unlock();

    //destory default connection
    QString name = QSqlDatabase::database().connectionName();
    QSqlDatabase::removeDatabase(name);
}

QSqlDatabase ConnectionPool::getConnection() {
    ConnectionPool& pool = ConnectionPool::getInstance();
    QString connectionName;

    QMutexLocker locker(&mutex);
    //check current work thread connection
    auto currentThreadId = QThread::currentThreadId();
    if (pool.keepConnections.contains(currentThreadId)) {
        connectionName = pool.keepConnections.value(currentThreadId);
    } else {
        if (!pool.unusedConnectionNames.empty()) {
            //use unused connection
            connectionName = pool.unusedConnectionNames.dequeue();
        } else {
            //create new connection name
            auto connectionCount = pool.keepConnections.size();
            connectionName = QString("Connection-%1").arg(connectionCount + 1);
        }
    }

    // open connection
    auto db = ConnectionPool::createConnection(connectionName);

    // push valid connection into usedConnectionNames
    if (db.isOpen()) {
        pool.keepConnections.insert(currentThreadId, connectionName);
        Q_ASSERT_X(pool.keepConnections.size() <= MAX_CONNECTION_SIZE, "ConnectionPool::openConnection", "forget to release worker connections?");
    }

    return db;
}

void ConnectionPool::closeConnection() {
    ConnectionPool& pool = ConnectionPool::getInstance();
    auto currentThreadId = QThread::currentThreadId();
    // close and remove when current work thread have connection
    if (pool.keepConnections.contains(currentThreadId)) {
        mutex.lock();
        auto connectionName = pool.keepConnections.take(currentThreadId);
        pool.unusedConnectionNames.enqueue(connectionName);
        QSqlDatabase::removeDatabase(connectionName);
        mutex.unlock();
    }
}

int ConnectionPool::getUsedConnectionSize() {
    QMutexLocker lock(&mutex);
    return getInstance().keepConnections.size() + getInstance().unusedConnectionNames.size();
}

QSqlDatabase ConnectionPool::createConnection(const QString &connectionName) {
    // check connection name in QSqlDatabase
    QSqlError lastErr;
    int testCount = 3;
    while (testCount--) {
        if (QSqlDatabase::contains(connectionName)) {
            {
                auto db1 = QSqlDatabase::database(connectionName);
                QSqlQuery query("select 1", db1);
                if (query.lastError().type() == QSqlError::NoError) {
                    return db1;
                }
            }
            //remove error connection, and try to create new connection
            QSqlDatabase::removeDatabase(connectionName);
        }

        // create new connection
        auto db = prepareConnect(connectionName, globalConfig->mDatabaseName);
        if (!db.open()) {
            lastErr = db.lastError();
        } else {
            return db;
        }
    }
    auto lastErrText = lastErr.text();
#ifdef QT_DEBUG
    qFatal(reinterpret_cast<const char *>(("create connection fail: " + lastErrText).data()));
#else
    Q_UNUSED(lastErrText)
#endif
    throw DaoException(lastErr);
}

QSqlDatabase ConnectionPool::prepareConnect(const QString& connectName, const QString& dbName) {
    return globalConfig->getConnection(connectName, dbName);
}


#include "connectionpool.h"
#include <qthread.h>

#include "dbexception.h"
#include "config/configbuilder.h"

#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>

QTDAO_USING_NAMESPACE

/**
 * open and save a new connection with 'name'
 * @param name current thread connection name
 */
ConnectionData::ConnectionData(QString name)
    : connectionName(std::move(name))
{
    openNewConnection();
}

/**
 * remove current thread used connection, and push into used names
 */
ConnectionData::~ConnectionData() {
    ConnectionPool::closeConnection(connectionName);
}

/**
 * get saved connection, and test connection useful
 * @return opened connection
 */
QSqlDatabase ConnectionData::getOpenedConnection() {
    testConnection();
    return QSqlDatabase::database(connectionName);
}

/**
 * create new database connect use current bind connection name
 */
void ConnectionData::openNewConnection() {
    ConnectionPool::createConnection(connectionName);
}

/**
 * test saved connection can be to use
 */
void ConnectionData::testConnection() {
    //test connection useful
    if (QSqlDatabase::contains(connectionName)) {
        {
            auto db = QSqlDatabase::database(connectionName);
            QSqlQuery query("select 1", db);
            if (query.lastError().type() == QSqlError::NoError) {
                return;
            }
        }
        QSqlDatabase::removeDatabase(connectionName);
    }
    openNewConnection();
}

QThreadStorage<ConnectionData*> ConnectionPool::localConnection;

ConnectionPool::ConnectionPool()
    : usedConnectionSize(0)
{
}

ConnectionPool& ConnectionPool::getInstance() {
    static ConnectionPool connectionPool;
    return connectionPool;
}

/**
 * get current thread saved database connection
 * @return opened connection
 */
QSqlDatabase ConnectionPool::getConnection() {

    if (!localConnection.hasLocalData()) {
        auto& pool = ConnectionPool::getInstance();
        QMutexLocker locker(&pool.mutex);

        QString connectionName;
        bool hasMoreUnusedNames = !pool.unusedConnectionNames.empty();
        if (hasMoreUnusedNames) {
            //use unused connection
            connectionName = pool.unusedConnectionNames.dequeue();
            MESSAGE_DEBUG("use recycled connection:", connectionName);
        } else {
            //create new connection name
            connectionName = QString("Connection-%1").arg(pool.usedConnectionSize + 1);
            MESSAGE_DEBUG("use new connection:", connectionName);
        }

        try {
            auto data = new ConnectionData(connectionName);
            localConnection.setLocalData(data);
            if (!hasMoreUnusedNames) {
                pool.usedConnectionSize++;
                MESSAGE_DEBUG("used size changed:", pool.usedConnectionSize);
            }
        } catch (DaoException& e) {
            throw e;
        }
    }

    return localConnection.localData()->getOpenedConnection();
}

/**
 * close opened connection of all thread
 */
void ConnectionPool::closeAllConnection() {
    auto& pool = ConnectionPool::getInstance();
    QMutexLocker locker(&pool.mutex);

    auto connections = QSqlDatabase::connectionNames();
    for (const auto& name : connections) {
        QSqlDatabase::removeDatabase(name);
    }

    pool.unusedConnectionNames.clear();
}

int ConnectionPool::getUsedConnectionSize() {
    auto& pool = ConnectionPool::getInstance();
    QMutexLocker locker(&pool.mutex);
    return pool.usedConnectionSize;
}

/**
 * close connect of current thread, and push into unused names
 * @param connectionName
 */
void ConnectionPool::closeConnection(const QString& connectionName) {
    auto& pool = ConnectionPool::getInstance();
    QMutexLocker locker(&pool.mutex);

    pool.unusedConnectionNames.enqueue(connectionName);
    QSqlDatabase::removeDatabase(connectionName);

    MESSAGE_DEBUG("recycle connection:", connectionName);
}

/**
 * open new database connection
 * @param connectionName
 * @return opened connection
 */
QSqlDatabase ConnectionPool::createConnection(const QString &connectionName) {
    // check connection name in QSqlDatabase
    QSqlError lastErr;
    int testCount = 3;
    while (testCount--) {
        {
            QSqlDatabase db;
            if (QSqlDatabase::contains(connectionName)) {
                db = QSqlDatabase::database(connectionName);
            } else {
                // create new connection
                db = prepareConnect(connectionName, globalConfig->mDatabaseName);
            }
            if (db.open()) {
                //test opened connection
                QSqlQuery query("select 1", db);
                if (query.lastError().type() == QSqlError::NoError) {
                    return db;
                }
            }
            lastErr = db.lastError();
        }
        QSqlDatabase::removeDatabase(connectionName);
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


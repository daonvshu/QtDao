#include "connectionpool.h"
#include <qthread.h>

#include "dbexception.h"
#include "config/configbuilder.h"
#include "config/configmanager.h"

#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>

QTDAO_USING_NAMESPACE

/**
 * open and save a new connection with 'name'
 * @param name current thread connection name
 */
ConnectionData::ConnectionData(QString name)
    : baseConnectionName(std::move(name))
{}

/**
 * remove current thread used connection, and push into used names
 */
ConnectionData::~ConnectionData() {
    ConnectionPool::closeConnection(baseConnectionName, usedConnectionNames.values());
}

/**
 * get saved connection, and test connection useful
 * @param sessionId session for different database connection in current thread
 * @return opened connection
 */
QSqlDatabase ConnectionData::getOpenedConnection(qint64 sessionId) {
    if (sessionId == -1 && !holdSessionIdStack.isEmpty()) {
        currentProcessSessionId = holdSessionIdStack.top();
    } else {
        currentProcessSessionId = sessionId;
    }
    testConnection();
    return QSqlDatabase::database(usedConnectionNames.value(currentProcessSessionId));
}

/**
 * push new session id use by scope connection
 * @param sessionId
 */
void ConnectionData::beginSession(qint64 sessionId) {
    holdSessionIdStack.push(sessionId);
}

/**
 * pop session id use by scope connection
 */
void ConnectionData::endSession() {
    if (!holdSessionIdStack.isEmpty()) {
        holdSessionIdStack.pop();
    }
}

/**
 * read top session id
 */
qint64 ConnectionData::stackTopSession() {
    if (holdSessionIdStack.isEmpty()) {
        return -1;
    }
    return holdSessionIdStack.top();
}

/**
 * create new database connect use current bind connection name
 */
void ConnectionData::openNewConnection() {
    auto connectionName = nameForDatabase();
    ConnectionPool::createConnection(currentProcessSessionId, connectionName);
    usedConnectionNames.insert(currentProcessSessionId, connectionName);
}

/**
 * test saved connection can be to use
 */
void ConnectionData::testConnection() {
    //test connection useful
    if (usedConnectionNames.contains(currentProcessSessionId)) {
        const auto& connectionName = usedConnectionNames.value(currentProcessSessionId);
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
        usedConnectionNames.remove(currentProcessSessionId);
    }
    openNewConnection();
}

/**
 * create connection name used by session id
 * @return
 */
QString ConnectionData::nameForDatabase() const {
    return baseConnectionName + "#" + QString::number(currentProcessSessionId);
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
QSqlDatabase ConnectionPool::getConnection(qint64 sessionId) {
    initializeLocalData();
    return localConnection.localData()->getOpenedConnection(sessionId);
}

/**
 * push new session id in stack
 * @param sessionId
 */
void ConnectionPool::scopeSessionBegin(qint64 sessionId) {
    Q_ASSERT(sessionId != -1);
    initializeLocalData();
    localConnection.localData()->beginSession(sessionId);
}

/**
 * pop session id in stack
 */
void ConnectionPool::scopeSessionEnd() {
    initializeLocalData();
    localConnection.localData()->endSession();
}

/**
 * get current thread session id, it is in top of session id stack, or return -1 if is empty
 * @return
 */
qint64 ConnectionPool::scopeCurrentSessionId() {
    initializeLocalData();
    return localConnection.localData()->stackTopSession();
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

/**
 * close all connections reference to the session id
 * @param sessionId
 */
void ConnectionPool::closeConnection(qint64 sessionId) {
    auto connectNameSuffix = "#" + QString::number(sessionId);

    auto connections = QSqlDatabase::connectionNames();
    for (const auto& name : connections) {
        if (name.endsWith(connectNameSuffix)) {
            QSqlDatabase::removeDatabase(name);
        }
    }
    //It is not necessary to recycle the connection name here
}

int ConnectionPool::getUsedConnectionSize() {
    auto& pool = ConnectionPool::getInstance();
    QMutexLocker locker(&pool.mutex);
    return pool.usedConnectionSize;
}

/**
 * check current thread data is not empty, or create new connection data use recycled connection
 */
void ConnectionPool::initializeLocalData() {
    if (!localConnection.hasLocalData()) {
        QString connectionName;

        { //locked only read connection name
            auto &pool = ConnectionPool::getInstance();
            QMutexLocker locker(&pool.mutex);
            if (!pool.unusedConnectionNames.empty()) {
                //use unused connection
                connectionName = pool.unusedConnectionNames.dequeue();
                MESSAGE_DEBUG("use recycled connection:", connectionName);
            } else {
                //create new connection name
                connectionName = QString("Connection-%1").arg(++pool.usedConnectionSize);
                MESSAGE_DEBUG("use new connection:", connectionName);
            }
        }
        localConnection.setLocalData(new ConnectionData(connectionName));
    }
}

/**
 * close connect of current thread, and push into unused names
 * @param connectionName name used by pool
 * @param nameInDatabase name used by database
 */
void ConnectionPool::closeConnection(const QString& connectionName, const QStringList& nameInDatabase) {
    auto& pool = ConnectionPool::getInstance();
    QMutexLocker locker(&pool.mutex);

    pool.unusedConnectionNames.enqueue(connectionName);
    for (const auto& name : nameInDatabase) {
        if (QSqlDatabase::contains(name)) {
            QSqlDatabase::removeDatabase(name);
        }
    }

    MESSAGE_DEBUG("recycle connection:", connectionName);
}

/**
 * open new database connection
 * @param connectionName
 * @return opened connection
 */
QSqlDatabase ConnectionPool::createConnection(qint64 sessionId, const QString &connectionName) {
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
                db = prepareConnect(connectionName, sessionId);
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

QSqlDatabase ConnectionPool::prepareConnect(const QString& connectName, qint64 sessionId) {
    return ConfigManager::getConfig(sessionId)->getConnection(connectName);
}


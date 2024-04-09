#pragma once

#include <QSqlDatabase>
#include <QQueue>
#include <QStack>
#include <QString>
#include <QMutex>
#include <QMutexLocker>
#include <QHash>
#include <QThreadStorage>
#include <QScopedPointer>

class ConnectionData {
public:
    explicit ConnectionData(QString name);
    ~ConnectionData();

    QSqlDatabase getOpenedConnection(qint64 sessionId);

    void beginSession(qint64 sessionId);
    void endSession();
    qint64 stackTopSession();

private:
    QString baseConnectionName;
    QHash<qint64, QString> usedConnectionNames;
    QStack<qint64> holdSessionIdStack;

    qint64 currentProcessSessionId = -1;

private:
    void openNewConnection();
    void testConnection();

    QString nameForDatabase() const;
};

class ConnectionPool {
public:
    static QSqlDatabase getConnection(qint64 sessionId = -1); // get current thread connection

    static void scopeSessionBegin(qint64 sessionId); // begin use session id in current thread
    static void scopeSessionEnd(); // end use session id in current thread
    static qint64 scopeCurrentSessionId(); // read current thread session id

    static void closeAllConnection(); // close all opened connection
    static void closeConnection(qint64 sessionId = -1); // close all connections reference to the session id

    static int getUsedConnectionSize();// get current used connection names

    static QSqlDatabase prepareConnect(const QString& connectName, qint64 sessionId);

private:
    ConnectionPool();

    static ConnectionPool& getInstance();

    static void initializeLocalData();
    static void closeConnection(const QString& connectionName, const QStringList& nameInDatabase);// close target connection name, and push into unused names
    static QSqlDatabase createConnection(qint64 sessionId, const QString &connectionName); // create database connection

    QQueue<QString> unusedConnectionNames;
    int usedConnectionSize;
    QMutex mutex;

    static QThreadStorage<ConnectionData*> localConnection;

    friend class ConnectionData;
};

class ConnectionScopedSessionControl {
public:
    explicit ConnectionScopedSessionControl(qint64 sessionId) {
        ConnectionPool::scopeSessionBegin(sessionId);
    }

    ~ConnectionScopedSessionControl() {
        ConnectionPool::scopeSessionEnd();
    }
};

#define DAO_SCOPED_SESSION_BEGIN(sessionId)  QScopedPointer<ConnectionScopedSessionControl> scopedSession##sessionId(new ConnectionScopedSessionControl(sessionId))
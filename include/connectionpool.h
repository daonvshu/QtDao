#pragma once

#include <QtSql/QSqlDatabase>
#include <QQueue>
#include <QString>
#include <QMutex>
#include <QMutexLocker>
#include <QHash>
#include <QThreadStorage>

class ConnectionData {
public:
    explicit ConnectionData(QString name);
    ~ConnectionData();

    QSqlDatabase getOpenedConnection();

private:
    QString connectionName;

private:
    void openNewConnection();
    void testConnection();
};

class ConnectionPool {
public:
    static QSqlDatabase getConnection(); // get current thread connection
    static void closeAllConnection(); // close all opened connection
    static int getUsedConnectionSize();// get current used connection names

    static QSqlDatabase prepareConnect(const QString & connectName, const QString & dbName);

private:
    ConnectionPool();

    static ConnectionPool& getInstance();

    static void closeConnection(const QString& connectionName);// close target connection name, and push into unused names
    static QSqlDatabase createConnection(const QString &connectionName); // create database connection

    QQueue<QString> unusedConnectionNames;
    int usedConnectionSize;
    QMutex mutex;

    static QThreadStorage<ConnectionData*> localConnection;

    friend class ConnectionData;
};

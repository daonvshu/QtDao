#pragma once

#include <QtSql/QSqlDatabase>
#include <QQueue>
#include <QString>
#include <QMutex>
#include <QMutexLocker>
#include <QHash>

class ConnectionPool {
public:
	static void release(); // close all database connection
	static QSqlDatabase getConnection(); // get current thread connection
	static void closeConnection(); // close the current work thread connection and push back into the connection pool
	static int getUsedConnectionSize();// get current used connection names

	~ConnectionPool();

	static QSqlDatabase prepareConnect(const QString & connectName, const QString & dbName);

private:
	static ConnectionPool& getInstance();

	ConnectionPool();
	QSqlDatabase createConnection(const QString &connectionName); // create database connection

	QQueue<QString> unusedConnectionNames;
	QHash<Qt::HANDLE, QString> keepConnections;// save work thread connections

	static QMutex mutex;
	static ConnectionPool *instance;
};

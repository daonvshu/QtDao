#pragma once

#include <QtSql>
#include <QQueue>
#include <QString>
#include <QMutex>
#include <QMutexLocker>

class ConnectionPool {
public:
	static void release(); // 关闭所有的数据库连接
	static QSqlDatabase openConnection();                 // 获取数据库连接
	static void closeConnection(); // 释放数据库连接回连接池

	~ConnectionPool();

	static QSqlDatabase prepareConnect(const QString & connectName, const QString & dbName);

private:
	static ConnectionPool& getInstance();

	ConnectionPool();
	QSqlDatabase createConnection(const QString &connectionName); // 创建数据库连接

	QQueue<QString> unusedConnectionNames; // 未使用的数据库连接名
	QMap<Qt::HANDLE, QString> keepConnections;//保存不同线程中的连接

	static QMutex mutex;
	static ConnectionPool *instance;
};

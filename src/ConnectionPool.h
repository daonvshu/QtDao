#pragma once

#include <QtSql>
#include <QQueue>
#include <QString>
#include <QMutex>
#include <QMutexLocker>

class ConnectionPool {
public:
	static void release(); // 关闭所有的数据库连接
	static QSqlDatabase getConnection(); // 获取数据库连接
	static void closeConnection(); // 释放当前数据库连接回连接池
	static int getUsedConnectionSize();//获取当前已经使用过的连接名数量

	~ConnectionPool();

	static QSqlDatabase prepareConnect(const QString & connectName, const QString & dbName);

private:
	static ConnectionPool& getInstance();

	ConnectionPool();
	QSqlDatabase createConnection(const QString &connectionName); // 创建数据库连接

	QQueue<QString> unusedConnectionNames; // 未使用的数据库连接名
	QHash<Qt::HANDLE, QString> keepConnections;//保存不同线程中的连接

	static QMutex mutex;
	static ConnectionPool *instance;
};

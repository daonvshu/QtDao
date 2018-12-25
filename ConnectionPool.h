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
	static void closeConnection(QSqlDatabase connection); // 释放数据库连接回连接池

	~ConnectionPool();

	static void loadConfigure(const QString& fileName);
	static QSqlDatabase prepareConnect(const QString & connectName, const QString & dbName);

	static bool isSqlite() { return sqlCfg.dbType == "QSQLITE"; }
	static bool isMysql() { return sqlCfg.dbType == "QMYSQL"; }
	static bool isSqlServer() { return sqlCfg.dbType == "QODBC"; }
	static QString getDbName() { return sqlCfg.dbName; }
	static int getDbVersion() { return sqlCfg.version; }

private:
	struct SqlCfg {
		int version;
		QString dbType;
		QString dbName;
		QString dbHost;
		QString dbUName;
		QString dbPcc;
		int dbPort;
	};

	static SqlCfg sqlCfg;

private:
	static ConnectionPool& getInstance();

	ConnectionPool();
	ConnectionPool(const ConnectionPool &other);
	ConnectionPool& operator=(const ConnectionPool &other);
	QSqlDatabase createConnection(const QString &connectionName); // 创建数据库连接

	QQueue<QString> usedConnectionNames;   // 已使用的数据库连接名
	QQueue<QString> unusedConnectionNames; // 未使用的数据库连接名

										   // 数据库信息
	bool    testOnBorrow;    // 取得连接的时候验证连接是否有效
	QString testOnBorrowSql; // 测试访问数据库的 SQL

	int maxWaitTime;  // 获取连接最大等待时间
	int waitInterval; // 尝试获取连接时等待间隔时间
	int maxConnectionCount; // 最大连接数

	static QMutex mutex;
	static QWaitCondition waitConnection;
	static ConnectionPool *instance;
};

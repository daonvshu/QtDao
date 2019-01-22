#include "ConnectionPool.h"
#include <qstandardpaths.h>
#include <qthread.h>
#include "DbLoader.h"
#include <qdebug.h>
//http://blog.csdn.net/qq_28796345/article/details/51586330
QMutex ConnectionPool::mutex;
QWaitCondition ConnectionPool::waitConnection;
ConnectionPool* ConnectionPool::instance = nullptr;

ConnectionPool::ConnectionPool() {
	testOnBorrow = true;
	testOnBorrowSql = "SELECT 1";

	maxWaitTime = 1000;
	waitInterval = 200;
	maxConnectionCount = 100;
}

ConnectionPool::~ConnectionPool() {
	// 销毁连接池的时候删除所有的连接
	mutex.lock();
	foreach(QString connectionName, usedConnectionNames) {
		QSqlDatabase::removeDatabase(connectionName);
	}

	foreach(QString connectionName, unusedConnectionNames) {
		QSqlDatabase::removeDatabase(connectionName);
	}
	mutex.unlock();

	QString name = QSqlDatabase::database().connectionName();
	QSqlDatabase::removeDatabase(name);

	connectionThreadId.clear();
}

ConnectionPool& ConnectionPool::getInstance() {
	if (nullptr == instance) {
		QMutexLocker locker(&mutex);
		if (nullptr == instance) {
			instance = new ConnectionPool();
		}
	}

	return *instance;
}

void ConnectionPool::release() {
	delete instance;
	instance = nullptr;
}

QSqlDatabase ConnectionPool::openConnection() {
	ConnectionPool& pool = ConnectionPool::getInstance();
	QString connectionName;

	QMutexLocker locker(&mutex);
	// 已创建连接数
	int connectionCount = pool.unusedConnectionNames.size() + pool.usedConnectionNames.size();

	// 如果连接已经用完，等待 waitInterval 毫秒看看是否有可用连接，最长等待 maxWaitTime 毫秒
	for (int i = 0;
		i < pool.maxWaitTime
		&& pool.unusedConnectionNames.empty() && connectionCount == pool.maxConnectionCount;
		i += pool.waitInterval) {
		waitConnection.wait(&mutex, pool.waitInterval);

		// 重新计算已创建连接数
		connectionCount = pool.unusedConnectionNames.size() + pool.usedConnectionNames.size();
	}
	if (!pool.unusedConnectionNames.empty()) {
		// 有已经回收的连接，复用它们
		//mutex.lock();
		connectionName = pool.unusedConnectionNames.dequeue();
		//mutex.unlock();
	}
	else if (connectionCount < pool.maxConnectionCount) {
		// 没有已经回收的连接，但是没有达到最大连接数，则创建新的连接
		connectionName = QString("Connection-%1").arg(connectionCount + 1);
	}
	else {
		// 已经达到最大连接数
		qDebug() << "Cannot create more connections.";
		return QSqlDatabase();
	}

	// 创建连接
	QSqlDatabase db = pool.createConnection(connectionName);
	//qDebug() << "use connectionName:" << connectionName;

	// 有效的连接才放入 usedConnectionNames
	if (db.isOpen()) {
		pool.usedConnectionNames.enqueue(connectionName);
		pool.connectionThreadId.insert(connectionName, QThread::currentThreadId());
	}

	return db;
}

void ConnectionPool::closeConnection(QSqlDatabase connection) {
	ConnectionPool& pool = ConnectionPool::getInstance();
	QString connectionName = connection.connectionName();
	//qDebug() << "close connection:" << connectionName;
	// 如果是我们创建的连接，从 used 里删除，放入 unused 里
	if (pool.usedConnectionNames.contains(connectionName)) {
		mutex.lock();
		pool.usedConnectionNames.removeOne(connectionName);
		pool.unusedConnectionNames.enqueue(connectionName);
		mutex.unlock();
		waitConnection.wakeOne();
	}
}

QSqlDatabase ConnectionPool::createConnection(const QString &connectionName) {
	// 检查连接名上次对应的线程是否是当前线程
	if (connectionThreadId.contains(connectionName)) {
		if (connectionThreadId.value(connectionName) != QThread::currentThreadId()) {
			QSqlDatabase::removeDatabase(connectionName);
			connectionThreadId.remove(connectionName);
		}
	}

	// 连接已经创建过了，复用它，而不是重新创建
	if (QSqlDatabase::contains(connectionName)) {
		QSqlDatabase db1 = QSqlDatabase::database(connectionName);
		db1.setHostName(DbLoader::getConfigure().dbHost);

		if (testOnBorrow) {
			// 返回连接前访问数据库，如果连接断开，重新建立连接
			//qDebug() << "Test connection on borrow, execute:" << testOnBorrowSql << ", for" << connectionName;
			QSqlQuery query(testOnBorrowSql, db1);

			if (query.lastError().type() != QSqlError::NoError || !db1.open()) {
				qDebug() << "Open datatabase error:" << db1.lastError().text();
				return QSqlDatabase();
			}
		}

		return db1;
	}

	// 创建一个新的连接
	QSqlDatabase db = prepareConnect(connectionName, DbLoader::getConfigure().dbName);
	if (!db.open()) {
		qDebug() << "Open datatabase error:" << db.lastError().text();
		return QSqlDatabase();
	}

	return db;
}

QSqlDatabase ConnectionPool::prepareConnect(const QString& connectName, const QString& dbName) {
	QSqlDatabase db = QSqlDatabase::addDatabase(DbLoader::getConfigure().dbType, connectName);
	if (DbLoader::isSqlite()) {
		db.setDatabaseName(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "\\" + dbName + ".db");
	} else {
		db.setDatabaseName(dbName);
		db.setUserName(DbLoader::getConfigure().dbUName);
		db.setPassword(DbLoader::getConfigure().dbPcc);
	}
	if (!DbLoader::isSqlite()) {
		db.setHostName(DbLoader::getConfigure().dbHost);
		db.setPort(DbLoader::getConfigure().dbPort);
	}
	if (!DbLoader::getConfigure().dbOption.isEmpty()) {
		db.setConnectOptions(DbLoader::getConfigure().dbOption);
	}
	return db;
}


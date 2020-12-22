#include "ConnectionPool.h"
#include <qstandardpaths.h>
#include <qthread.h>

#include "DbLoader.h"
#include "DbExceptionHandler.h"

#define MAX_CONNECTION_SIZE	200

QMutex ConnectionPool::mutex;
ConnectionPool* ConnectionPool::instance = nullptr;

ConnectionPool::ConnectionPool() {
}

ConnectionPool::~ConnectionPool() {
	// 销毁连接池的时候删除所有的连接
	mutex.lock();
	foreach(QString connectionName, keepConnections) {
		QSqlDatabase::removeDatabase(connectionName);
	}

	foreach(QString connectionName, unusedConnectionNames) {
		QSqlDatabase::removeDatabase(connectionName);
	}
	mutex.unlock();

	//销毁默认连接
	QString name = QSqlDatabase::database().connectionName();
	QSqlDatabase::removeDatabase(name);
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

QSqlDatabase ConnectionPool::getConnection() {
	ConnectionPool& pool = ConnectionPool::getInstance();
	QString connectionName;

	QMutexLocker locker(&mutex);
	//查询当前线程是否保持有连接
	auto currentThreadId = QThread::currentThreadId();
	if (pool.keepConnections.contains(currentThreadId)) {
		connectionName = pool.keepConnections.value(currentThreadId);
	} else {
		if (!pool.unusedConnectionNames.empty()) {
			//复用线程已回收连接
			connectionName = pool.unusedConnectionNames.dequeue();
		} else {
			//创建新的连接名
			auto connectionCount = pool.keepConnections.size();
			connectionName = QString("Connection-%1").arg(connectionCount + 1);
		}
	}

	// 创建连接
	auto db = pool.createConnection(connectionName);

	// 有效的连接才放入 usedConnectionNames
	if (db.isOpen()) {
		pool.keepConnections.insert(currentThreadId, connectionName);
		Q_ASSERT_X(pool.keepConnections.size() <= MAX_CONNECTION_SIZE, "ConnectionPool::openConnection", "forget to release worker connections?");
	}

	return db;
}

void ConnectionPool::closeConnection() {
	ConnectionPool& pool = ConnectionPool::getInstance();
	auto currentThreadId = QThread::currentThreadId();
	// 如果当前线程有连接，关闭连接并移除
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
	// 连接已经创建过了，复用它，而不是重新创建
	if (QSqlDatabase::contains(connectionName)) {
		auto db1 = QSqlDatabase::database(connectionName);
		QSqlQuery query("select 1", db1);
		if (query.lastError().type() != QSqlError::NoError) {
			DbExceptionHandler::exceptionHandler->databaseOpenFail(query.lastError().text());
			return QSqlDatabase();
		}
		return db1;
	}

	// 创建一个新的连接
	auto db = prepareConnect(connectionName, DbLoader::getConfig().dbName);
	if (!db.open()) {
		DbExceptionHandler::exceptionHandler->databaseOpenFail(db.lastError().text());
		return QSqlDatabase();
	}

	return db;
}

QSqlDatabase ConnectionPool::prepareConnect(const QString& connectName, const QString& dbName) {
	QSqlDatabase db = QSqlDatabase::addDatabase(DbLoader::getConfig().dbDriver, connectName);
	if (DbLoader::getConfig().isSqlite()) {
		db.setDatabaseName(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "\\" + dbName + ".db");
		if (!DbLoader::getConfig().dbPcc.isEmpty()) {
			db.setPassword(DbLoader::getConfig().dbPcc);
		}
	} 
	else if (DbLoader::getConfig().isMysql()) {
		db.setDatabaseName(dbName);
		db.setUserName(DbLoader::getConfig().dbUName);
		db.setPassword(DbLoader::getConfig().dbPcc);
		db.setHostName(DbLoader::getConfig().dbHost);
		db.setPort(DbLoader::getConfig().dbPort);
	}
	else if (DbLoader::getConfig().isSqlServer()) {
		db.setDatabaseName(QString("DRIVER={SQL SERVER};SERVER=%1;DATABASE=%2").arg(DbLoader::getConfig().dbHost).arg(dbName));
		db.setUserName(DbLoader::getConfig().dbUName);
		db.setPassword(DbLoader::getConfig().dbPcc);
	}

	if (!DbLoader::getConfig().dbOption.isEmpty()) {
		db.setConnectOptions(DbLoader::getConfig().dbOption);
	}
	return db;
}


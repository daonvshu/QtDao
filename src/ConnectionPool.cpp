#include "ConnectionPool.h"
#include <qstandardpaths.h>
#include <qthread.h>

#include "DbLoader.h"
#include "DbExceptionHandler.h"

#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>

#define MAX_CONNECTION_SIZE	200

QMutex ConnectionPool::mutex;
ConnectionPool* ConnectionPool::instance = nullptr;

ConnectionPool::ConnectionPool() {
}

ConnectionPool::~ConnectionPool() {
	// close all connection when destoryed, call release()
	mutex.lock();
	foreach(QString connectionName, keepConnections) {
		QSqlDatabase::removeDatabase(connectionName);
	}

	foreach(QString connectionName, unusedConnectionNames) {
		QSqlDatabase::removeDatabase(connectionName);
	}
	mutex.unlock();

	//destory default connection
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
	//check current work thread connection
	auto currentThreadId = QThread::currentThreadId();
	if (pool.keepConnections.contains(currentThreadId)) {
		connectionName = pool.keepConnections.value(currentThreadId);
	} else {
		if (!pool.unusedConnectionNames.empty()) {
			//use unused connection
			connectionName = pool.unusedConnectionNames.dequeue();
		} else {
			//create new connection name
			auto connectionCount = pool.keepConnections.size();
			connectionName = QString("Connection-%1").arg(connectionCount + 1);
		}
	}

	// open connection
	auto db = pool.createConnection(connectionName);

	// push valid connection into usedConnectionNames
	if (db.isOpen()) {
		pool.keepConnections.insert(currentThreadId, connectionName);
		Q_ASSERT_X(pool.keepConnections.size() <= MAX_CONNECTION_SIZE, "ConnectionPool::openConnection", "forget to release worker connections?");
	}

	return db;
}

void ConnectionPool::closeConnection() {
	ConnectionPool& pool = ConnectionPool::getInstance();
	auto currentThreadId = QThread::currentThreadId();
	// close and remove when current work thread have connection
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
	// check connection name in QSqlDatabase
	if (QSqlDatabase::contains(connectionName)) {
		{
			auto db1 = QSqlDatabase::database(connectionName);
			QSqlQuery query("select 1", db1);
			if (query.lastError().type() == QSqlError::NoError) {
				return db1;
			}
		}
		//remove error connection, and try to create new connection
		QSqlDatabase::removeDatabase(connectionName);
	}

	// create new connection
	auto db = prepareConnect(connectionName, DbLoader::getConfig().dbName);
	if (!db.open()) {
		if (DbExceptionHandler::exceptionHandler) {
			DbExceptionHandler::exceptionHandler->databaseOpenFail(DbErrCode::CREATE_CONNECTION_FAIL, db.lastError().text());
		}
		Q_ASSERT_X(DbExceptionHandler::exceptionHandler != nullptr, "ConnectionPool", "open database fail!");
		return QSqlDatabase();
	}

	return db;
}

QSqlDatabase ConnectionPool::prepareConnect(const QString& connectName, const QString& dbName) {
	QSqlDatabase db = QSqlDatabase::addDatabase(DbLoader::getConfig().dbDriver, connectName);
	if (DbLoader::getConfig().isSqlite()) {
		db.setDatabaseName(DbLoader::getConfig().getDbStorePath());
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


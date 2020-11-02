#include "ConnectionPool.h"
#include <qstandardpaths.h>
#include <qthread.h>
#include <qdebug.h>

#include "DbLoader.h"

QMutex ConnectionPool::mutex;
ConnectionPool* ConnectionPool::instance = nullptr;

ConnectionPool::ConnectionPool() {
}

ConnectionPool::~ConnectionPool() {
	// �������ӳص�ʱ��ɾ�����е�����
	mutex.lock();
	foreach(QString connectionName, keepConnections) {
		QSqlDatabase::removeDatabase(connectionName);
	}

	foreach(QString connectionName, unusedConnectionNames) {
		QSqlDatabase::removeDatabase(connectionName);
	}
	mutex.unlock();

	//����Ĭ������
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

QSqlDatabase ConnectionPool::openConnection() {
	ConnectionPool& pool = ConnectionPool::getInstance();
	QString connectionName;

	QMutexLocker locker(&mutex);
	//��ѯ��ǰ�߳��Ƿ񱣳�������
	auto currentThreadId = QThread::currentThreadId();
	if (pool.keepConnections.contains(currentThreadId)) {

	}
	
	// �Ѵ���������
	int connectionCount = pool.unusedConnectionNames.size() + pool.usedConnectionNames.size();

	// ��������Ѿ����꣬�ȴ� waitInterval ���뿴���Ƿ��п������ӣ���ȴ� maxWaitTime ����
	for (int i = 0;
		i < pool.maxWaitTime
		&& pool.unusedConnectionNames.empty() && connectionCount == pool.maxConnectionCount;
		i += pool.waitInterval) {
		waitConnection.wait(&mutex, pool.waitInterval);

		// ���¼����Ѵ���������
		connectionCount = pool.unusedConnectionNames.size() + pool.usedConnectionNames.size();
	}
	if (!pool.unusedConnectionNames.empty()) {
		// ���Ѿ����յ����ӣ���������
		//mutex.lock();
		connectionName = pool.unusedConnectionNames.dequeue();
		//mutex.unlock();
	}
	else if (connectionCount < pool.maxConnectionCount) {
		// û���Ѿ����յ����ӣ�����û�дﵽ������������򴴽��µ�����
		connectionName = QString("Connection-%1").arg(connectionCount + 1);
	}
	else {
		// �Ѿ��ﵽ���������
		qDebug() << "Cannot create more connections.";
		return QSqlDatabase();
	}

	// ��������
	QSqlDatabase db = pool.createConnection(connectionName);
	//qDebug() << "use connectionName:" << connectionName;

	// ��Ч�����Ӳŷ��� usedConnectionNames
	if (db.isOpen()) {
		pool.usedConnectionNames.enqueue(connectionName);
		//pool.connectionThreadId.insert(connectionName, QThread::currentThreadId());
	}

	return db;
}

void ConnectionPool::closeConnection(const QSqlDatabase& connection) {
	ConnectionPool& pool = ConnectionPool::getInstance();
	QString connectionName = connection.connectionName();
	// ��������Ǵ��������ӣ��� used ��ɾ�������� unused ��
	if (pool.usedConnectionNames.contains(connectionName)) {
		mutex.lock();
		pool.usedConnectionNames.removeOne(connectionName);
		pool.unusedConnectionNames.enqueue(connectionName);
		mutex.unlock();
		waitConnection.wakeOne();
	}
}

void ConnectionPool::closeConnection(const QString& connectionName) {
	ConnectionPool& pool = ConnectionPool::getInstance();
	if (QThread::currentThread() != qApp->thread()) {
		//�ͷŹ����߳�����
		QSqlDatabase::removeDatabase(connectionName);
	}
	if (pool.usedConnectionNames.contains(connectionName)) {
		mutex.lock();
		pool.usedConnectionNames.removeOne(connectionName);
		pool.unusedConnectionNames.enqueue(connectionName);
		mutex.unlock();
		waitConnection.wakeOne();
	}
}

QSqlDatabase ConnectionPool::createConnection(const QString &connectionName) {
	// ����������ϴζ�Ӧ���߳��Ƿ��ǵ�ǰ�߳�
	/*if (connectionThreadId.contains(connectionName)) {
		if (connectionThreadId.value(connectionName) != QThread::currentThreadId()) {
			QSqlDatabase::removeDatabase(connectionName);
			connectionThreadId.remove(connectionName);
		}
	}*/

	// �����Ѿ��������ˣ������������������´���
	if (QSqlDatabase::contains(connectionName)) {
		QSqlDatabase db1 = QSqlDatabase::database(connectionName);
		db1.setHostName(DbLoader::getConfig().dbHost);

		if (testOnBorrow) {
			// ��������ǰ�������ݿ⣬������ӶϿ������½�������
			//qDebug() << "Test connection on borrow, execute:" << testOnBorrowSql << ", for" << connectionName;
			QSqlQuery query(testOnBorrowSql, db1);

			if (query.lastError().type() != QSqlError::NoError || !db1.open()) {
				qDebug() << "Open datatabase error:" << db1.lastError().text();
				return QSqlDatabase();
			}
		}

		return db1;
	}

	// ����һ���µ�����
	QSqlDatabase db = prepareConnect(connectionName, DbLoader::getConfig().dbName);
	if (!db.open()) {
		qDebug() << "Open datatabase error:" << db.lastError().text();
		return QSqlDatabase();
	}

	return db;
}

QSqlDatabase ConnectionPool::prepareConnect(const QString& connectName, const QString& dbName) {
	QSqlDatabase db = QSqlDatabase::addDatabase(DbLoader::getConfig().dbType, connectName);
	if (DbLoader::getConfig().isSqlite()) {
		db.setDatabaseName(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "\\" + dbName + ".db");
	} else {
		if (DbLoader::getConfig().isSqlServer()) {
			db.setDatabaseName(QString("DRIVER={SQL SERVER};SERVER=%1;DATABASE=%2")
				.arg(DbLoader::getConfig().dbHost).arg(dbName));
		} else {
			db.setDatabaseName(dbName);
		}
		db.setUserName(DbLoader::getConfig().dbUName);
		db.setPassword(DbLoader::getConfig().dbPcc);
	}
	if (DbLoader::getConfig().isMysql()) {
		db.setHostName(DbLoader::getConfig().dbHost);
		db.setPort(DbLoader::getConfig().dbPort);
	}
	if (!DbLoader::getConfig().dbOption.isEmpty()) {
		db.setConnectOptions(DbLoader::getConfig().dbOption);
	}
	return db;
}

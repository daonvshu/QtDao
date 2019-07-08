#include "DaoUtil.h"

dao::ErrPrintType dao::pType = dao::ETYPE_CONSOLE;
int dao::bindCount = 0;

DbLoader::SqlCfg DbLoader::sqlCfg;
DbLoader::SqlClient* DbLoader::sqlClient = nullptr;

dao::DaoExecutor::DaoExecutor(const QString & sql_head, const QString & tableName, ExecutorData* executorData)
{
	this->sql_head = sql_head;
	this->tableName = tableName;
	this->executorData = executorData;
}

void dao::DaoExecutor::concatSqlStatement()
{
	if (!getWriteEntity().getKvPair().isEmpty()) {
		sql_head.append(" set ").append(getWriteEntity().getKvPair());
	}
	if (!getReadEntity().getKvPair().isEmpty()) {
		sql_head.append(" where ").append(getReadEntity().getKvPair());
	}
	if (!getExtraEntity().getKvPair().isEmpty()) {
		sql_head.append(getExtraEntity().getKvPair());
	}
}

void dao::DaoExecutor::mergeValueList()
{
	valueList.append(getWriteEntity().getVList());
	valueList.append(getReadEntity().getVList());
}

void dao::DaoExecutor::bindTableName()
{
	sql_head.append(tableName);
}

void dao::DaoExecutor::bindValue(QSqlQuery & query)
{
	concatSqlStatement();
	query.prepare(sql_head);
	mergeValueList();
	for (const auto& d : valueList) {
		query.addBindValue(d, d.type() == QMetaType::QByteArray ? QSql::Binary : QSql::In);
	}
}

dao::DaoJoinExecutor::DaoJoinExecutor(const QList<QStringList>& jps, const QString sql, const QStringList tbOrder, const QVariantList & valueList)
{
	this->joinParameters = jps;
	this->sql = sql;
	this->tbOrderList = tbOrder;
	this->valueList = valueList;
}

dao::DaoJoinExecutor::DaoJoinExecutor(const DaoJoinExecutor & executor)
{
	this->joinParameters = executor.joinParameters;
	this->sql = executor.sql;
	this->tbOrderList = executor.tbOrderList;
	this->valueList = executor.valueList;
}

QList<dao::DaoJoinExecutor::DaoJoinExecutorItem> dao::DaoJoinExecutor::list()
{
	QList<DaoJoinExecutorItem> dataItems;
	auto db = ConnectionPool::openConnection();
	QSqlQuery query(db);
	query.prepare(sql);
	for (const auto& a : valueList) {
		query.addBindValue(a);
	}
	if (!query.exec()) {
		dao::printLog(query.lastError().text(), sql);
	} else {
		while (query.next()) {
			QVariantList data;
			auto record = query.record();
			for (int i = 0; i < record.count(); i++) {
				data.append(record.value(i));
			}
			dataItems.append(DaoJoinExecutorItem(data, new DaoJoinExecutor(*this)));
		}
	}
	ConnectionPool::closeConnection(db);
	return dataItems;
}

dao::DaoJoinExecutor dao::SqlJoinBuilder::build()
{
	QList<QStringList> joinParameters;
	QString sql = "select ";
	QVariantList valueList;
	QStringList tbList;

	int count = 0;
	for (auto& entities : joinEntities) {
		QStringList fields;
		QString no = ('a' + count++);
		for (auto& entity : entities) {
			sql.append(no).append('.').append(entity()).append(',');
			fields.append(entity());
		}
		joinParameters.append(fields);
	}
	sql = sql.left(sql.length() - 1);

	auto firstInfo = joinInfos.takeFirst();
	auto firstEntity = joinReadEntities.takeFirst();
	Q_ASSERT(firstEntity.name().startsWith("a."));//forget to call dao::bindJoinOrder?
	tbList.append(firstInfo.tbName);

	sql.append(" from ").append(firstInfo.tbName).append(" a");
	for (int i = 0; i < joinInfos.size(); i++) {
		auto info = joinInfos.at(i);
		auto readEntity = joinReadEntities.at(i);

		QString joinStr;
		switch (info.joinType) {
		default:
		case JOIN_LEFT:
			joinStr = " left join ";
			break;
		case JOIN_RIGHT:
			joinStr = " right join ";
			break;
		case JOIN_INNER:
			joinStr = " inner join ";
			break;
		case JOIN_FULL_OUTER:
			joinStr = " full outer join ";
			break;
		}
		QString no = ('a' + (i + 1));
		sql.append(joinStr).append(info.tbName).append(' ').append(no);
		if (!readEntity.getKvPair().isEmpty()) {
			sql.append(" on ").append(readEntity.getKvPair());
		}
		valueList.append(readEntity.getVList());
		tbList.append(info.tbName);
	}
	if (!firstEntity.getKvPair().isEmpty()) {
		sql.append(" where ").append(firstEntity.getKvPair());
		valueList.append(firstEntity.getVList());
	}

	return DaoJoinExecutor(joinParameters, sql, tbList, valueList);
}

void dao::printLog(const QString & lastErr, const QString & sql)
{
	Q_ASSERT(lastErr.isEmpty());
	if (pType == ETYPE_NONE)
		return;
	QString errStr = "--------sql error--------\r\nlast error:%1\r\n";
	errStr = errStr.arg(lastErr);
	switch (pType) {
	case dao::ETYPE_FILE:
	{
		QFile outFile("debug.log");
		outFile.open(QIODevice::WriteOnly | QIODevice::Append);
		QTextStream ts(&outFile);
		QString curTime = "[%1]";
		ts << errStr << curTime.arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss:zzz")) << endl;
	}
	break;
	case dao::ETYPE_CONSOLE:
	{
		auto splitStr = errStr.split("\r\n");
		for (const auto& s : splitStr) {
			qDebug() << s;
		}
	}
	break;
	case dao::ETYPE_ERRBOX:
	{
		auto e = new QErrorMessage;
		e->setWindowTitle(QStringLiteral("sql´íÎó"));
		e->showMessage(errStr);
	}
	break;
	default:
		break;
	}
}

QVariant DbLoader::readDbSetting(const QString & key, const QString & default)
{
	QString iniFilePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/db.ini";
	QSettings settings(iniFilePath, QSettings::IniFormat);
	return settings.value("Db/" + key, default);
}

void DbLoader::writeDbSetting(const QString & key, const QVariant & value)
{
	QString iniFilePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/db.ini";
	QSettings settings(iniFilePath, QSettings::IniFormat);
	settings.setValue("Db/" + key, value);
}

void DbLoader::loadConfigure(const QString & fileName)
{
	QDomDocument doc;
	QFile file(fileName);
	auto s = file.open(QIODevice::ReadOnly);
	Q_ASSERT(s);
	s = doc.setContent(&file);
	Q_ASSERT(s);
	file.close();
	auto root = doc.documentElement();

	auto client = root.attribute("type");
	if (client == "mysql") {
		sqlCfg.dbType = "QMYSQL";
		sqlClient = new MysqlClient;
	} else if (client == "sqlite") {
		sqlCfg.dbType = "QSQLITE";
		sqlClient = new SqliteClient;
	} else if (client == "sqlserver") {
		sqlCfg.dbType = "QODBC";
		sqlClient = new SqlServerClient;
	}

	sqlCfg.version = root.attribute("version").toInt();
	sqlCfg.dbName = root.attribute("dbname");

	auto options = root.childNodes();
	for (int i = 0; i < options.count(); i++) {
		auto c = options.at(i).toElement();
		if (c.tagName() == "dbname") {
			sqlCfg.dbName = c.text();
		} else if (c.tagName() == "dbhost") {
			sqlCfg.dbHost = c.text();
		} else if (c.tagName() == "dbuname") {
			sqlCfg.dbUName = c.text();
		} else if (c.tagName() == "dbpcc") {
			sqlCfg.dbPcc = c.text();
		} else if (c.tagName() == "dbport") {
			sqlCfg.dbPort = c.text().toInt();
		} else if (c.tagName() == "dboption") {
			sqlCfg.dbOption = c.text();
		}
	}
	if (sqlCfg.dbHost.isEmpty()) {
		sqlCfg.dbHost = readDbSetting(DB_SETTING_HOST, "localhost").toString();
	}
}

bool DbCreatorHelper::testConnect()
{
	if (DbLoader::isSqlite()) {
		auto appLocal = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
		QDir dir;
		if (!dir.exists(appLocal)) {
			dir.mkdir(appLocal);
		}
		return true;
	}
	if (DbLoader::isMysql()) {
		auto db = ConnectionPool::prepareConnect("testMysql", "mysql");
		if (!db.open()) {
			qDebug() << "connect mysql fail! err = " << db.lastError();
			return false;
		}
		auto db2 = ConnectionPool::prepareConnect("testDb", DbLoader::getConfigure().dbName);
		if (!db2.open()) {
			//create target database
			QSqlQuery query(db);
			QString sql = "create database if not exists %1 default character set utf8 COLLATE utf8_general_ci";
			sql = sql.arg(DbLoader::getConfigure().dbName);
			if (!query.exec(sql)) {
				qDebug() << "create target database fail! err = " << db.lastError();
				db.close();
				return false;
			}
		}
		db.close();
		db2.close();
		return true;
	} else if (DbLoader::isSqlServer()) {
		auto db = ConnectionPool::prepareConnect("testSqlServer", DbLoader::getConfigure().dbName);
		if (!db.open()) {
			qDebug() << "connect sqlserver fail! err = " << db.lastError();
			return false;
		}
		db.close();
		return true;
	}
	return false;
}

bool DbCreatorHelper::checkDbVersion()
{
	return DbLoader::readDbSetting(DB_SETTING_VERSION, "-1").toInt() == DbLoader::getConfigure().version;
}

void DbCreatorHelper::resetDbVersion()
{
	DbLoader::writeDbSetting(DB_SETTING_VERSION, DbLoader::getConfigure().version);
}

bool DbCreatorHelper::checkTableExist(const QString & tbName)
{
	bool exist = false;
	execSql(DbLoader::getClient()->findTableStatement(tbName), [&](auto& query) {
		if (query.next()) {
			exist = true;
		}
	}, [](auto& lastErr) {
		qDebug() << "check table exist fail, err = " << lastErr;
	});
	return exist;
}

bool DbCreatorHelper::renameTable(const QString & oldTb, const QString & newTb)
{
	bool success = true;
	execSql(DbLoader::getClient()->renameTableStatement(oldTb, newTb), [&](auto& lastErr) {
		qDebug() << "rename table fail, err = " << lastErr;
		success = false;
	});
	return success;
}

bool DbCreatorHelper::dropTable(const QString & tb)
{
	QString sql = "drop table " + tb;
	bool success = true;
	execSql(sql, [&](auto& lastErr) {
		qDebug() << "drop table fail, err = " << lastErr;
		success = false;
	});
	return success;
}

bool DbCreatorHelper::truncateTable(const QString & tb)
{
	QString sql = "truncate table " + tb;
	bool success = true;
	execSql(sql, [&](auto& lastErr) {
		qDebug() << "truncate table fail, err = " << lastErr;
		success = false;
	});
	return success;
}
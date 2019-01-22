#pragma once

#include <qobject.h>
#include <qdom.h>
#include <qsettings.h>
#include <qstandardpaths.h>
#include <qfile.h>

#define DB_SETTING_HOST		"host"
#define DB_SETTING_VERSION	"version"

class DbLoader {
private:
	class SqlClient {
	public:
		virtual QString findTableStatement(const QString& tbName) = 0;

		template<typename T>
		QString createTableStatement(T& entity) {
			QString sql = "create table %1(%2)";
			QString fieldsStr;
			auto fields = entity->getFieldsType();
			for (const auto& f : fields) {
				fieldsStr.append(f);
				fieldsStr.append(",");
			}
			fieldsStr = fieldsStr.left(fieldsStr.length() - 1);
			sql = sql.arg(entity->getTableName(), fieldsStr);
			if (DbLoader::isMysql()) {
				if (!entity->engine().isEmpty()) {
					sql.append(" engine=").append(entity->engine());
				}
				sql.append(" default charset=utf8");
			}
			return sql;
		}

		template<typename T>
		QString createIndexStatement(T& entity) {
			QStringList indexes = entity->getIndexFields();
			QString indexStr, indexName = "idx_";
			for (const auto& f : indexes) {
				indexStr.append(f).append(",");
				indexName.append(f).append("_");
			}
			indexStr = indexStr.left(indexStr.length() - 1);
			indexName = indexName.left(indexName.length() - 1);
			return createIndexStatement(entity->getTableName(), indexName, indexStr);
		}

		virtual QString renameTableStatement(const QString& oldTb, const QString& newTb) = 0;

	protected:
		virtual QString createIndexStatement(const QString& tbName, const QString& indexName, const QString& indexList) = 0;
	};

	class SqliteClient : public SqlClient {
	public:
		QString findTableStatement(const QString& tbName) override {
			return QString("select *from sqlite_master where type='table' and name = '%1'").arg(tbName);
		}

		QString renameTableStatement(const QString& oldTb, const QString& newTb) {
			return QString("alter table %1 rename to %2").arg(oldTb, newTb);
		}

	protected:
		QString createIndexStatement(const QString& tbName, const QString& indexName, const QString& indexList) {
			return QString("create unique index if not exists %1 on %2 (%3)").arg(indexName, tbName, indexList);
		}
	};

	class MysqlClient : public SqlClient {
	public:
		QString findTableStatement(const QString& tbName) override {
			return QString("select table_name from information_schema.TABLES where table_name ='%1' and table_schema = '%2'").arg(tbName, DbLoader::getConfigure().dbName);
		}

		QString renameTableStatement(const QString& oldTb, const QString& newTb) {
			return QString("alter table %1 rename to %2").arg(oldTb, newTb);
		}

	protected:
		QString createIndexStatement(const QString& tbName, const QString& indexName, const QString& indexList) {
			return QString("create unique index %1 on %2 (%3)").arg(indexName, tbName, indexList);
		}
	};

	class SqlServerClient : public SqlClient {
	public:
		QString findTableStatement(const QString& tbName) override {
			return QString("select * from sys.tables where name = '%1' and type = 'U'").arg(tbName);
		}

		QString renameTableStatement(const QString& oldTb, const QString& newTb) {
			return QString("exec sp_rename '%1','%2'").arg(oldTb, newTb);
		}

	protected:
		QString createIndexStatement(const QString& tbName, const QString& indexName, const QString& indexList) {
			return QString("create nonclustered index %1 on %2 (%3) with (ignore_dup_key=on,drop_existing=on)").arg(indexName, tbName, indexList);
		}
	};

private:
	struct SqlCfg {
		int version;
		QString dbType;
		QString dbName;
		QString dbHost;
		QString dbUName;
		QString dbPcc;
		int dbPort;
		QString dbOption;
	};

	static SqlCfg sqlCfg;
	static SqlClient* sqlClient;

public:
	static bool isSqlite() { return sqlCfg.dbType == "QSQLITE"; }
	static bool isMysql() { return sqlCfg.dbType == "QMYSQL"; }
	static bool isSqlServer() { return sqlCfg.dbType == "QODBC"; }

	static const SqlCfg& getConfigure() { return sqlCfg; }
	static SqlClient* getClient() { return sqlClient; }

	static QVariant readDbSetting(const QString& key, const QString& default) {
		QString iniFilePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/db.ini";
		QSettings settings(iniFilePath, QSettings::IniFormat);
		return settings.value("Db/" + key, default);
	}

	static void writeDbSetting(const QString& key, const QVariant& value) {
		QString iniFilePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/db.ini";
		QSettings settings(iniFilePath, QSettings::IniFormat);
		settings.setValue("Db/" + key, value);
	}

public:
	static void loadConfigure(const QString& fileName) {
		QDomDocument doc;
		QFile file(fileName);
		Q_ASSERT(file.open(QIODevice::ReadOnly));
		Q_ASSERT(doc.setContent(&file));
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
				sqlCfg.dbOption = c.text().toInt();
			}
		}
		if (sqlCfg.dbHost.isEmpty()) {
			sqlCfg.dbHost = readDbSetting(DB_SETTING_HOST, "localhost").toString();
		}
	}
	static void loadHost(const QString& host) {
		sqlCfg.dbHost = host;
	}
};
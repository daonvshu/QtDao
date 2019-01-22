/*
Descript:	数据库创建类，根据不同的版本号对数据库进行升级
Author:		daonvshu
Date:		2018/12/14
Last-Md:	2019/01/22
*/
#pragma once

#include <qdebug.h>
#include <qobject.h>
#include <qapplication.h>
#include "DbLoader.h"
#include "ConnectionPool.h"

class DbCreatorHelper {
public:
	static bool testConnect() {
		if (DbLoader::isSqlite())
			return true;
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

	static bool checkDbVersion() {
		return DbLoader::readDbSetting(DB_SETTING_VERSION, "-1").toInt() == DbLoader::getConfigure().version;
	}

	static void resetDbVersion() {
		DbLoader::writeDbSetting(DB_SETTING_VERSION, DbLoader::getConfigure().version);
	}

	static bool checkTableExist(const QString& tbName) {
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

	template<typename T>
	static bool createTable(T entity) {
		bool success = true;
		execSql(DbLoader::getClient()->createTableStatement(entity), [&](auto& lastErr) {
			qDebug() << "create table fail, err = " << lastErr;
			success = false;
		});

		if (success) {
			if (!entity->getIndexFields().isEmpty()) {
				execSql(DbLoader::getClient()->createIndexStatement(entity), [&](auto& lastErr) {
					qDebug() << "create index fail, err = " << lastErr;
					if (lastErr.nativeErrorCode() != "1061") {
						success = false;
					}
				});
			}
		}
		return success;
	}

	static bool renameTable(const QString& oldTb, const QString& newTb) {
		bool success = true;
		execSql(DbLoader::getClient()->renameTableStatement(oldTb, newTb), [&](auto& lastErr) {
			qDebug() << "rename table fail, err = " << lastErr;
			success = false;
		});
		return success;
	}

	template<typename T>
	static bool restoreData2NewTable(const QString& tmpTb, T entity) {
		bool success = true;
		execSql("select *from " + tmpTb, [&](auto& query) {
			auto record = query.record();
			QStringList newFields = entity->getFields();
			QString oldFields;
			for (int i = 0; i < record.count(); i++) {
				if (newFields.contains(record.fieldName(i))) {
					oldFields.append(record.fieldName(i)).append(",");
				}
			}
			oldFields = oldFields.left(oldFields.length() - 1);

			QString sql = "insert into %1(%2) select %2 from %3";
			sql = sql.arg(entity->getTableName(), oldFields, tmpTb);
			if (!query.exec(sql)) {
				success = false;
				qDebug() << "restore tmp data fail!";
			}
		}, [](auto& lastErr) {});

		return success;
	}

	static bool dropTable(const QString& tb) {
		QString sql = "drop table " + tb;
		bool success = true;
		execSql(sql, [&](auto& lastErr) {
			qDebug() << "drop table fail, err = " << lastErr;
			success = false;
		});
		return success;
	}

	static bool truncateTable(const QString& tb) {
		QString sql = "truncate table " + tb;
		bool success = true;
		execSql(sql, [&](auto& lastErr) {
			qDebug() << "truncate table fail, err = " << lastErr;
			success = false;
		});
		return success;
	}

private:
	template<typename F>
	static void execSql(const QString& sql, F fail) {
		execSql(sql, [](auto& query) {}, fail);
	}

	template<typename F1, typename F2>
	static void execSql(const QString& sql, F1 succ, F2 fail) {
		auto db = ConnectionPool::openConnection();
		QSqlQuery query(db);
		if (!query.exec(sql)) {
			succ(query);
		} else {
			fail(query.lastError());
		}
		ConnectionPool::closeConnection(db);
	}
};

template <typename... T> struct DbCreator;
template <typename H, typename... T>
struct DbCreator<H, T...> {
public:
	static void init(bool& success) {
		if (!success)
			return;
		auto entity = static_cast<H*>(0);

		bool tbExist = DbCreatorHelper::checkTableExist(entity->getTableName());
		QString tmpTbName = entity->getTableName().prepend("tmp_");
		if (!DbCreatorHelper::checkDbVersion() && tbExist) {//表存在且版本不同进行数据库升级
			if (!DbCreatorHelper::renameTable(entity->getTableName(), tmpTbName)) {
				success = false;
				return;
			}
		}
		if (!tbExist) {
			success = success && DbCreatorHelper::createTable(entity);
		}
		if (DbCreatorHelper::checkTableExist(tmpTbName)) {//转移数据到新表中
			if (!DbCreatorHelper::restoreData2NewTable(tmpTbName, entity)) {
				success = false;
				return;
			}
			DbCreatorHelper::dropTable(tmpTbName);//删除临时表
		}

		DbCreator<T...>::init(success);
	}
};
template<> struct DbCreator<> {
	static void init(bool& success) {
		if (success) {
			DbCreatorHelper::resetDbVersion();
		}
	} 
};
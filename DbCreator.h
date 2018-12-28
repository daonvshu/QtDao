/*
Descript:	数据库创建类，根据不同的版本号对数据库进行升级
Author:		daonvshu
Version:	2.0
Date:		2018/12/14
Last-Md:	2018/12/28
*/
#pragma once

#include <qdebug.h>
#include <qobject.h>
#include <qapplication.h>
#include "sql\ConnectionPool.h"

namespace DbCreatorHelper {
	bool testConnect() {
		if (ConnectionPool::isSqlite())
			return true;
		if (ConnectionPool::isMysql()) {
			auto db = ConnectionPool::prepareConnect("testMysql", "mysql");
			if (!db.open()) {
				qDebug() << "connect mysql fail! err = " << db.lastError();
				return false;
			}
			auto db2 = ConnectionPool::prepareConnect("testDb", ConnectionPool::getDbName());
			if (!db2.open()) {
				//create target database
				QSqlQuery query(db);
				QString sql = "create database if not exists %1 default character set utf8 COLLATE utf8_general_ci";
				sql = sql.arg(ConnectionPool::getDbName());
				if (!query.exec(sql)) {
					qDebug() << "create target database fail! err = " << db.lastError();
					db.close();
					return false;
				}
			}
			db.close();
			db2.close();
			return true;
		}
		return false;
	}

	bool checkDbVersion() {
		return ConnectionPool::readDbSetting("version", "-1").toInt() == ConnectionPool::getDbVersion();
	}

	void resetDbVersion() {
		ConnectionPool::writeDbSetting("version", ConnectionPool::getDbVersion());
	}

	bool checkTableExist(const QString& tbName) {
		QString sql;
		if (ConnectionPool::isSqlite()) {
			sql = "select *from sqlite_master where type='table' and name = '%1'";
		} else if (ConnectionPool::isMysql()) {
			sql = "select table_name from information_schema.TABLES where table_name ='%1'";
		} else {
			return false;
		}
		sql = sql.arg(tbName);
		
		bool exist = false;
		auto db = ConnectionPool::openConnection();
		QSqlQuery query(db);
		if (!query.exec(sql)) {
			qDebug() << "check table exist fail, err = " << query.lastError();
		} else {
			if (query.next()) {
				exist = true;
			}
		}
		ConnectionPool::closeConnection(db);
		return exist;
	}

	template<typename T>
	bool createTableIfNotExist(T entity) {
		QString sql = "create table if not exists %1(%2)";
		if (ConnectionPool::isMysql()) {
			sql.append(" default charset=utf8");
		}
		QString fieldsStr;
		auto fields = entity->getFieldsType();
		for (const auto& f : fields) {
			fieldsStr.append(f);
			fieldsStr.append(",");
		}
		fieldsStr = fieldsStr.left(fieldsStr.length() - 1);
		sql = sql.arg(entity->getTableName(), fieldsStr);
		
		bool success = true;
		auto db = ConnectionPool::openConnection();
		QSqlQuery query(db);
		if (!query.exec(sql)) {
			qDebug() << "create table fail, err = " << query.lastError();
			success = false;
		}
		QStringList indexes = entity->getIndexFields();
		if (!indexes.isEmpty()) {
			QString indexStr;
			for (const auto& f : indexes) {
				indexStr.append(f);
				indexStr.append(",");
			}
			indexStr = indexStr.left(indexStr.length() - 1);

			sql = "create unique index index_%1 on %1 (%2)";
			sql = sql.arg(entity->getTableName(), indexStr);
			if (!query.exec(sql)) {
				qDebug() << "create index fail, err = " << query.lastError();
				if (!query.lastError().text().contains("already exists") && query.lastError().nativeErrorCode() != "1061") {
					success = false;
				}
			}
		}

		ConnectionPool::closeConnection(db);
		return success;
	}

	bool renameTable(const QString& oldTb, const QString& newTb) {
		if (ConnectionPool::isSqlServer())
			return false;
		QString sql = "alter table %1 rename to %2";
		sql = sql.arg(oldTb, newTb);

		bool success = true;
		auto db = ConnectionPool::openConnection();
		QSqlQuery query(db);
		if (!query.exec(sql)) {
			qDebug() << "rename table fail, err = " << query.lastError();
			success = false;
		}
		ConnectionPool::closeConnection(db);
		return success;
	}

	template<typename T>
	bool restoreData2NewTable(const QString& tmpTb, T entity) {
		bool success = true;
		auto db = ConnectionPool::openConnection();
		QSqlQuery query(db);
		query.exec("select *from " + tmpTb);
		if (query.next()) {
			auto record = query.record();
			QStringList newFields = entity->getFields();
			QString oldFields;
			for (int i = 0; i < record.count(); i++) {
				if (newFields.contains(record.fieldName(i))) {
					oldFields.append(record.fieldName(i));
					oldFields.append(",");
				}
			}
			oldFields = oldFields.left(oldFields.length() - 1);

			QString sql = "insert into %1(%2) select %2 from %3";
			sql = sql.arg(entity->getTableName(), oldFields, tmpTb);
			if (!query.exec(sql)) {
				success = false;
				qDebug() << "restore tmp data fail!";
			}
		}
		ConnectionPool::closeConnection(db);
		return success;
	}

	bool dropTable(const QString& tb) {
		QString sql = "drop table %1";
		sql = sql.arg(tb);

		bool success = true;
		auto db = ConnectionPool::openConnection();
		QSqlQuery query(db);
		if (!query.exec(sql)) {
			qDebug() << "drop table fail, err = " << query.lastError();
			success = false;
		}
		ConnectionPool::closeConnection(db);
		return success;
	}
}

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
		success = success && DbCreatorHelper::createTableIfNotExist(entity);
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
/*
Descript:	sql执行类
Author:		daonvshu
Version:	2.2
Date:		2018/12/14
Last-Md:	2019/01/25
*/
#pragma once

#include <QObject>
#include "ConnectionPool.h"
#include <qdebug.h>
#include <qfile.h>
#include <qerrormessage.h>
#include "dao\DaoEntityField.h"

#define PRINT_SQL_STATEMENT

class dao {
private:
	enum OperateType {
		OPERATE_QUERY,
		OPERATE_INSERT,
		OPERATE_INSERT_OR_REPLACE,
		OPERATE_UPDATE,
		OPERATE_DELETE,
		OPERATE_COUNT,
	};

	template<class T>
	class DaoExecutor;

	template<typename E, typename K = DaoExecutor<E>>
	class SqlBuilder;

	template<class T>
	class DaoExecutor {
	public:
		DaoExecutor() = delete;
		DaoExecutor &operator=(const DaoExecutor&) = delete;

		DaoExecutor(const QString& sql_head, const QString& tableName, SqlBuilder<T, DaoExecutor<T>>* builder = nullptr) {
			this->sql_head = sql_head;
			this->tableName = tableName;
			this->builder = builder;
		}

	protected:
		void mergeQueryInfoCopy(DaoExecutor& other) {
			other.bindTableName();
			other.concatSqlStatement();
			other.mergeValueList();
			tableName = QString("(%1) as res_%2").arg(other.sql_head, other.tableName);
			valueList.append(other.valueList);
		}

		/*executor prepare*/
		void concatSqlStatement() {
			if (!getWriteEntity().getKvPair().isEmpty()) {
				sql_head.append(" set ").append(getWriteEntity().getKvPair());
			}
			if (!getReadEntity().getKvPair().isEmpty()) {
				sql_head.append(" where ").append(getReadEntity().getKvPair());
			}
		}

		void mergeValueList() {
			valueList.append(getWriteEntity().getVList());
			valueList.append(getReadEntity().getVList());
		}

		void bindTableName() {
			sql_head.append(tableName);
		}

		void bindValue(QSqlQuery& query) {
			concatSqlStatement();
			query.prepare(sql_head);
			mergeValueList();
			for (const auto& d : valueList) {
				query.addBindValue(d, d.type() == QMetaType::QByteArray ? QSql::Binary : QSql::In);
			}
		}

		/*sql executor*/
		template<class F>
		bool exec(F success) {
			return exec(false, success);
		}

		bool exec() {
			return exec(false, [] (auto& query) {});
		}

		template<class F>
		bool execBatch(F success) {
			return exec(true, success);
		}

		bool execBatch() {
			return exec(true, [] (auto& query) {});
		}

		/*build information*/
		DaoEntityField& getWriteEntity() {
			return builder->writeEntity;
		}

		DaoEntityField& getReadEntity() {
			return builder->readEntity;
		}

		QList<DaoEntityField>& getBindEntities() {
			return builder->bindEntities;
		}

		QStringList& getExtraDataFields() {
			return builder->bindExtraName;
		}

	protected:
		QString sql_head, tableName;
		SqlBuilder<T, DaoExecutor<T>>* builder;
		QVariantList valueList;

	private:
		template<class F>
		bool exec(bool batch, F success) {
			auto db = ConnectionPool::openConnection();
			QSqlQuery query(db);
			bindValue(query);
#ifdef PRINT_SQL_STATEMENT
			qDebug() << sql_head;
#endif // PRINT_SQL_STATEMENT
			bool ok = batch ? query.execBatch() : query.exec();
			if (!ok) {
				dao::printLog(query.lastError().text(), sql_head);
			} else {
				success(query);
			}
			ConnectionPool::closeConnection(db);
			return ok;
		}
	};

	template<class T>
	class DaoCountExecutor : public DaoExecutor<T> {
	public:
		using DaoExecutor::DaoExecutor;

		int count() {
			int count = 0;
			bindTableName();
			exec([&] (auto& query) {
				query.next();
				count = query.value(0).toInt();
			});
			return count;
		}

		DaoCountExecutor& mergeQuery(DaoExecutor& other) {
			mergeQueryInfoCopy(other);
			return *this;
		}
	};

	template<class T>
	class DaoQueryExecutor : public DaoCountExecutor<T> {
	public:
		using DaoCountExecutor::DaoCountExecutor;

		T unique(bool& exist) {
			Q_ASSERT(getWriteEntity().getKvPair().isEmpty());
			T entity;
			exist = false;

			bindTableName();
			exec([&](auto& query) {
				if (query.next()) {
					exist = true;
					auto record = query.record();
					if (getBindEntities().isEmpty()) {
						for (const auto& p : entity.getFields()) {
							entity.bindValue(p, record.value(p));
						}
					} else {
						for (auto& p : getBindEntities()) {
							entity.bindValue(p.name(), record.value(p.name()));
						}
						if (!getExtraDataFields().isEmpty()) {
							for (const auto& e : getExtraDataFields()) {
								entity.putExtraData(e, record.value(e));
							}
						}
					}
				}
			});
			return entity;
		}

		QList<T> list() {
			Q_ASSERT(getWriteEntity().getKvPair().isEmpty());
			QList<T> entities;
			T entity;

			bindTableName();
			exec([&](auto& query) {
				while (query.next()) {
					auto record = query.record();
					if (getBindEntities().isEmpty()) {
						for (const auto& p : entity.getFields()) {
							entity.bindValue(p, record.value(p));
						}
					} else {
						for (auto& p : getBindEntities()) {
							entity.bindValue(p.name(), record.value(p.name()));
						}
						if (!getExtraDataFields().isEmpty()) {
							for (const auto& e : getExtraDataFields()) {
								entity.putExtraData(e, record.value(e));
							}
						}
					}
					entities.append(entity);
				}
			});
			return entities;
		}

		DaoQueryExecutor& mergeQuery(DaoExecutor& other) {
			mergeQueryInfoCopy(other);
			return *this;
		}
	};

	template<class T>
	class DaoInsertExecutor : public DaoExecutor<T> {
	public:
		using DaoExecutor::DaoExecutor;

		bool insert(T& entity) {
			Q_ASSERT(getReadEntity().getKvPair().isEmpty() && getWriteEntity().getKvPair().isEmpty());
			bindTableName();
			sql_head.append(" values(%1)");
			QVariantList entityData = entity.readEntity();
			QString prepareStr = QString("?,").repeated(entityData.size());
			prepareStr = prepareStr.left(prepareStr.length() - 1);
			sql_head = sql_head.arg(prepareStr);

			for (const auto& d : entityData) {
				valueList.append(d);
			}

			return exec([&](auto& query) {
				entity.bindId(query.lastInsertId().toInt());
			});;
		}

		bool insertMutil(const QList<T>& entities) {
			if (entities.isEmpty()) {
				return true;
			}
			Q_ASSERT(getReadEntity().getKvPair().isEmpty() && getWriteEntity().getKvPair().isEmpty());
			bindTableName();
			sql_head.append(" values");
			int fieldSize = static_cast<T*>(0)->fieldSize();
			QString prepareStr = QString("?,").repeated(fieldSize);
			prepareStr = prepareStr.left(prepareStr.length() - 1);
			for (int i = 0; i < entities.size(); i++) {
				sql_head.append(QString("(").append(prepareStr).append("),"));
			}
			sql_head = sql_head.left(sql_head.length() - 1);
			for (const auto& entity : entities) {
				valueList.append(entity.readEntity());
			}
			return exec();
		}

		bool insertBatch(const QList<T>& entities) {
			if (entities.isEmpty()) {
				return true;
			}
			Q_ASSERT(getReadEntity().getKvPair().isEmpty() && getWriteEntity().getKvPair().isEmpty());
			bindTableName();
			sql_head.append(" values(%1)");
			int fieldSize = static_cast<T*>(0)->fieldSize();
			QString prepareStr = QString("?,").repeated(fieldSize);
			prepareStr = prepareStr.left(prepareStr.length() - 1);
			sql_head = sql_head.arg(prepareStr);

			QList<QVariantList> dataList;
			for (int i = 0; i < fieldSize; i++) {
				dataList << QVariantList();
			}
			for (const auto& entity : entities) {
				QVariantList entityData = entity.readEntity();
				int k = 0;
				for (const auto& d : entityData) {
					auto dlist = (QVariantList*)&dataList.at(k++);
					*dlist << d;
				}
			}
			for (int i = 0; i < fieldSize; i++) {
				valueList.append(QVariant(dataList.at(i)));
			}

			return execBatch();
		}

		DaoInsertExecutor& mergeQuery(DaoExecutor& other) {
			mergeQueryInfoCopy(other);
			return *this;
		}
	};

	template<class T>
	class DaoUpdateExecutor : public DaoExecutor<T> {
	public:
		using DaoExecutor::DaoExecutor;

		/*used by 'set' and 'where' conditions to update table */
		bool update() {
			Q_ASSERT(!getWriteEntity().getKvPair().isEmpty() && !getReadEntity().getKvPair().isEmpty());
			bindTableName();
			return exec();
		}
		/*used by 'set' and 'where' conditions to update table, value list must be QVariantList */
		bool updateBatch() {
			Q_ASSERT(!getWriteEntity().getKvPair().isEmpty() && !getReadEntity().getKvPair().isEmpty());
			bindTableName();
			return execBatch();
		}
		/*used by 'where' conditions to update entity to table*/
		bool update(T& entity) {
			Q_ASSERT(getWriteEntity().getKvPair().isEmpty() && !getReadEntity().getKvPair().isEmpty());

			bindTableName();
			QVariantList entityData = entity.readEntity();
			QStringList fields = static_cast<T*>(0)->getFields();
			QString field_id = static_cast<T*>(0)->getIdField();
			QString setPa = " set ";
			bool findId = false;
			for (int i = 0; i < fields.size(); i++) {
				if (fields.at(i) == field_id) {
					findId = true;
					continue;
				}
				valueList.append(entityData.at(i));
				setPa.append(fields.at(i));
				setPa.append(" = ?, ");
			}
			Q_ASSERT(findId);//查询表没有id字段
			sql_head.append(setPa.left(setPa.length() - 2));

			return exec();
		}
		/*used by binded fields(condition from entity by fields) to update entity to table*/
		bool updateBy(T& entity) {
			Q_ASSERT(!getBindEntities().isEmpty());
			Q_ASSERT(getWriteEntity().getKvPair().isEmpty() && getReadEntity().getKvPair().isEmpty());

			bindTableName();
			QVariantList entityData = entity.readEntity();
			QStringList fields = static_cast<T*>(0)->getFields();
			QString setPa = " set ", whPa = " where ";

			QStringList bindFieldList;
			for (auto& entity : getBindEntities()) {
				bindFieldList.append(entity());
			}

			QVariantList cndVList;
			bool findId = false;
			QString field_id = static_cast<T*>(0)->getIdField();
			for (int i = 0; i < fields.size(); i++) {
				if (fields.at(i) == field_id) {
					findId = true;
					continue;
				}
				if (!bindFieldList.contains(fields.at(i))) {
					valueList.append(entityData.at(i));
					setPa.append(fields.at(i));
					setPa.append(" = ?, ");
				} else {
					cndVList.append(entityData.at(i));
					whPa.append(fields.at(i)).append(" = ? and ");
				}
			}
			Q_ASSERT(findId);//查询表没有id字段
			sql_head.append(setPa.left(setPa.length() - 2) + whPa.left(whPa.length() - 5));
			valueList.append(cndVList);
			return exec();
		}
		/*used by binded fields(condition from entities by fields) to update entities to table*/
		bool updateBatch(QList<T>& entities) {
			Q_ASSERT(!getBindEntities().isEmpty());
			Q_ASSERT(getWriteEntity().getKvPair().isEmpty() && getReadEntity().getKvPair().isEmpty());
			if (entities.isEmpty())
				return true;

			bindTableName();
			QStringList fields = static_cast<T*>(0)->getFields();
			int fieldSize = fields.size();

			QStringList bindFieldList;
			for (auto& entity : getBindEntities()) {
				bindFieldList.append(entity());
			}

			QMap<int, QVariantList> updataList;
			QMap<int, QVariantList> cddataList;

			bool findId = false;
			QString field_id = static_cast<T*>(0)->getIdField();
			for (const auto& entity : entities) {
				QVariantList entityData = entity.readEntity();
				int k = 0;
				for (const auto& d : entityData) {
					if (fields.at(k) == field_id) {
						findId = true;
						k++;
						continue;
					}
					if (!bindFieldList.contains(fields.at(k))) {
						auto data = updataList.value(k);
						data << d;
						updataList.insert(k++, data);
					} else {
						auto data = cddataList.value(k);
						data << d;
						cddataList.insert(k++, data);
					}
				}
			}
			Q_ASSERT(findId);//查询表没有id字段

			QString setCond = " set ";
			QString whCond = " where ";
			for (int i = 0; i < fields.size(); i++) {
				if (fields.at(i) == field_id) {
					continue;
				}
				if (!bindFieldList.contains(fields.at(i))) {
					setCond.append(fields.at(i));
					setCond.append(" = ?, ");
				} else {
					whCond.append(fields.at(i));
					whCond.append(" = ? and ");
				}
			}

			sql_head.append(setCond.left(setCond.length() - 2)).append(whCond.left(whCond.length() - 4));

			for (int i = 0; i < fieldSize; i++) {
				if (updataList.contains(i)) {
					valueList.append(QVariant(updataList.value(i)));
				}
			}
			for (int i = 0; i < fieldSize; i++) {
				if (cddataList.contains(i)) {
					valueList.append(QVariant(cddataList.value(i)));
				}
			}

			return execBatch();
		}

		DaoUpdateExecutor& mergeQuery(DaoExecutor& other) {
			mergeQueryInfoCopy(other);
			return *this;
		}
	};

	template<class T>
	class DaoDeleteExecutor : public DaoExecutor<T> {
	public:
		using DaoExecutor::DaoExecutor;

		bool deleteBy() {
			bindTableName();
			return exec();
		}

		bool deleteBatch() {
			bindTableName();
			return execBatch();
		}

		DaoDeleteExecutor& mergeQuery(DaoExecutor& other) {
			mergeQueryInfoCopy(other);
			return *this;
		}
	};

	template<typename E, typename K>
	class SqlBuilder {
	private:
		OperateType operateType;
		DaoEntityField writeEntity, readEntity;
		QList<DaoEntityField> bindEntities;
		QStringList bindExtraName;

		template<typename T>
		friend DaoEntityField& DaoExecutor<T>::getWriteEntity();

		template<typename T>
		friend DaoEntityField& DaoExecutor<T>::getReadEntity();

		template<typename T>
		friend QList<DaoEntityField>& DaoExecutor<T>::getBindEntities();

		template<typename T>
		friend QStringList& DaoExecutor<T>::getExtraDataFields();

	public:
		SqlBuilder(OperateType type) : operateType(type) {}

		SqlBuilder& bind() {
			return *this;
		}
		/*bind multiply fields*/
		template<typename ...T>
		SqlBuilder& bind(DaoEntityField& p, T... t) {
			bindEntities.append(p);
			return bind(t...);
		}

		K build() {
			QString sql;
			if (operateType == OPERATE_QUERY) {
				if (bindEntities.isEmpty()) {
					sql = "select *from ";
				} else {
					QString bindEntitiesStr;
					for (auto& bindEntity : bindEntities) {
						bindEntitiesStr.append(bindEntity.nameExtra()).append(',');
						if (!bindEntity.asIsName()) {
							bindExtraName.append(bindEntity.asString());
						}
					}
					bindEntitiesStr = bindEntitiesStr.left(bindEntitiesStr.length() - 1);
					sql = "select " + bindEntitiesStr + " from ";
				}
			} else if (operateType == OPERATE_INSERT) {
				sql = "insert into ";
			} else if (operateType == OPERATE_INSERT_OR_REPLACE) {
				sql = "replace into ";
			} else if (operateType == OPERATE_UPDATE) {
				sql = "update ";
			} else if (operateType == OPERATE_DELETE) {
				sql = "delete from ";
			} else if (operateType == OPERATE_COUNT) {
				sql = "select count(*) from ";
			}
			return K(sql, static_cast<E*>(0)->getTableName(), (SqlBuilder<E, DaoExecutor<E>>*)(this));
		}

		SqlBuilder& wh(DaoEntityField& field) {
			this->readEntity = field;
			return *this;
		}

		SqlBuilder& set(DaoEntityField& field) {
			this->writeEntity = field;
			return *this;
		}
	};

	class DaoJoinExecutor {
	private:
		QList<QStringList> joinParameters;
		QStringList tbOrderList;

		QString sql;
		QVariantList valueList;

		class DaoJoinExecutorItem {
		private:
			DaoJoinExecutor* executor;
			QVariantList data;
			int bindCount = 0;
			int setCount = 0;

		public:
			DaoJoinExecutorItem(const QVariantList& data, DaoJoinExecutor* executor) {
				this->data = data;
				this->executor = executor;
			}

			void bind() {}

			template<typename K, typename ...T>
			void bind(K& entity, T&... entities) {
				Q_ASSERT(executor->tbOrderList.at(bindCount) == entity.getTableName());//bind实例顺序与查询表顺序不一致
				auto jp = executor->joinParameters.at(bindCount++);
				for (const auto& p : jp) {
					entity.bindValue(p, data.at(setCount++));
				}
				bind(entities...);
			}
		};

		friend class DaoJoinExecutorItem;

	public:
		DaoJoinExecutor(const QList<QStringList>& jps, const QString sql, const QStringList tbOrder, const QVariantList& valueList) {
			this->joinParameters = jps;
			this->sql = sql;
			this->tbOrderList = tbOrder;
			this->valueList = valueList;
		}

		QList<DaoJoinExecutorItem> list() {
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
					dataItems.append(DaoJoinExecutorItem(data, this));
				}
			}
			ConnectionPool::closeConnection(db);
			return dataItems;
		}
	};

	class SqlJoinBuilder {
	private:
		enum JoinType {
			JOIN_LEFT,
			JOIN_RIGHT,
			JOIN_INNER,
			JOIN_FULL_OUTER,
			JOIN_NULL,
		};

		struct JoinInfo {
			QString tbName;
			JoinType joinType;
		};
		QList<QList<DaoEntityField>> joinEntities;
		QList<DaoEntityField> tempJoinEntities;
		QList<JoinInfo> joinInfos;
		QList<DaoEntityField> joinReadEntities;

	private:
		template<class T>
		SqlJoinBuilder& join(JoinType joinType) {
			auto tbName = static_cast<T*>(0)->getTableName();
			joinInfos.append({ tbName, joinType });
			return *this;
		}

	public:
		SqlJoinBuilder& bind() {
			joinEntities.append(tempJoinEntities);
			tempJoinEntities.clear();
			return *this;
		}

		template<typename ...T>
		SqlJoinBuilder& bind(DaoEntityField& f, T... t) {
			tempJoinEntities.append(f);
			return bind(t...);
		}

		SqlJoinBuilder& wh(DaoEntityField& field) {
			joinReadEntities.append(field);
			return *this;
		}

		template<class T>
		SqlJoinBuilder& jleft() {
			return join<T>(JOIN_LEFT);
		}

		template<class T>
		SqlJoinBuilder& jright() {
			return join<T>(JOIN_RIGHT);
		}

		template<class T>
		SqlJoinBuilder& jinner() {
			return join<T>(JOIN_INNER);
		}

		template<class T>
		SqlJoinBuilder& jfull() {
			return join<T>(JOIN_FULL_OUTER);
		}

		template<class T>
		SqlJoinBuilder& jnull() {
			return join<T>(JOIN_NULL);
		}

		DaoJoinExecutor build() {
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
	};

public:
	template<typename E>
	static SqlBuilder<E, DaoQueryExecutor<E>> _query() {
		return SqlBuilder<E, DaoQueryExecutor<E>>(OPERATE_QUERY);
	}

	template<typename E>
	static SqlBuilder<E, DaoInsertExecutor<E>> _insert() {
		return SqlBuilder<E, DaoInsertExecutor<E>>(OPERATE_INSERT);
	}

	template<typename E>
	static SqlBuilder<E, DaoInsertExecutor<E>> _replace() {
		return SqlBuilder<E, DaoInsertExecutor<E>>(OPERATE_INSERT_OR_REPLACE);
	}

	template<typename E>
	static SqlBuilder<E, DaoUpdateExecutor<E>> _update() {
		return SqlBuilder<E, DaoUpdateExecutor<E>>(OPERATE_UPDATE);
	}

	template<typename E>
	static SqlBuilder<E, DaoDeleteExecutor<E>> _delete() {
		return SqlBuilder<E, DaoDeleteExecutor<E>>(OPERATE_DELETE);
	}

	template<typename E>
	static SqlBuilder<E, DaoCountExecutor<E>> _count() {
		return SqlBuilder<E, DaoCountExecutor<E>>(OPERATE_COUNT);
	}

	static SqlJoinBuilder _join() {
		return SqlJoinBuilder();
	}

	static void bindJoinOrder() {
		bindCount = 0;
	}

	template<typename K, typename... T>
	static void bindJoinOrder(K& k, T&... t) {
		k.bindJoin(bindCount++);
		bindJoinOrder(t...);
	}

private:
	static int bindCount;

private:
	enum ErrPrintType {
		ETYPE_NONE,
		ETYPE_FILE,
		ETYPE_CONSOLE,
		ETYPE_ERRBOX,
	};

	static ErrPrintType pType;

public:
	static void printLog(const QString& lastErr, const QString& sql) {
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
			e->setWindowTitle(QStringLiteral("sql错误"));
			e->showMessage(errStr);
		}
		break;
		default:
			break;
		}
	}
};
#pragma once

#include "ConnectionPool.h"
#include <qdebug.h>
#include <qfile.h>
#include <qerrormessage.h>
#include "dao\DaoEntityField.h"
#include <qobject.h>
#include <qdom.h>
#include <qsettings.h>
#include <qstandardpaths.h>
#include <qfile.h>
#include <qapplication.h>

#define DB_SETTING_HOST		"host"
#define DB_SETTING_VERSION	"version"

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

    struct ExecutorData {
        //operate (bindExtraName|*) (into|from) table (set writeEntity) (where readEntity extraEntity)
        OperateType operateType;
        DaoEntityField writeEntity, readEntity, extraEntity;
        QList<DaoEntityField> bindEntities;
        QStringList bindExtraName;
    };

    class DaoExecutor {
    public:
        DaoExecutor(const QString& sql_head, const QString& tableName, ExecutorData* executorData);

    protected:
        /*executor prepare*/
        void concatSqlStatement();
        void mergeValueList();
        void bindTableName();
        void bindValue(QSqlQuery& query);

        /*sql executor*/
        template<class F>
        bool exec(F success) {
            return exec(false, success);
        }

        bool exec() {
            return exec(false, [](auto& query) {});
        }

        template<class F>
        bool execBatch(F success) {
            return exec(true, success);
        }

        bool execBatch() {
            return exec(true, [](auto& query) {});
        }

        /*build information*/
        DaoEntityField& getWriteEntity() {
            return executorData->writeEntity;
        }

        DaoEntityField& getReadEntity() {
            return executorData->readEntity;
        }

        DaoEntityField& getExtraEntity() {
            return executorData->extraEntity;
        }

        QList<DaoEntityField>& getBindEntities() {
            return executorData->bindEntities;
        }

        QStringList& getExtraDataFields() {
            return executorData->bindExtraName;
        }

    protected:
        QString sql_head, tableName;
        ExecutorData* executorData;
        QVariantList valueList;

    private:
        template<class F>
        bool exec(bool batch, F success);
    };

    template<typename T>
    class DaoCountExecutor : public DaoExecutor {
    public:
        using DaoExecutor::DaoExecutor;

        int count();
    };

    template<typename T>
    class DaoQueryExecutor : public DaoExecutor {
    public:
        using DaoExecutor::DaoExecutor;

        T unique(bool& exist);

        QList<T> list();

        DaoQueryExecutor& mergeQuery(DaoQueryExecutor& other);
    };

    template<typename T>
    class DaoInsertExecutor : public DaoExecutor {
    public:
        using DaoExecutor::DaoExecutor;

        bool insert(T& entity);
        bool insertMutil(const QList<T>& entities);
        bool insertBatch(const QList<T>& entities);
    };

    template<typename T>
    class DaoUpdateExecutor : public DaoExecutor {
    public:
        using DaoExecutor::DaoExecutor;

        /*used by 'set' and 'where' conditions to update table */
        bool update();
        /*used by 'set' and 'where' conditions to update table, value list must be QVariantList */
        bool updateBatch();
        /*used by 'where' conditions to update entity to table*/
        bool update(T& entity);
        /*used by binded fields(condition from entity by fields) to update entity to table*/
        bool updateBy(T& entity);
        /*used by binded fields(condition from entities by fields) to update entities to table*/
        bool updateBatch(QList<T>& entities);
    };

    template<typename T>
    class DaoDeleteExecutor : public DaoExecutor {
    public:
        using DaoExecutor::DaoExecutor;

        bool deleteBy();
        bool deleteBatch();
    };

    template<typename E, typename K>
    class SqlBuilder {
    private:
        ExecutorData executorData;

    public:
        SqlBuilder(OperateType type) {
            executorData.operateType = type;
        }

        SqlBuilder& bind() {
            return *this;
        }
        /*bind multiply fields*/
        template<typename ...T>
        SqlBuilder& bind(DaoEntityField& p, T... t) {
            executorData.bindEntities.append(p);
            return bind(t...);
        }

        K build();

        SqlBuilder& wh(DaoEntityField& field) {
            executorData.readEntity = field;
            return *this;
        }

        SqlBuilder& set(DaoEntityField& field) {
            executorData.writeEntity = field;
            return *this;
        }

        SqlBuilder& extra(DaoEntityField& field) {
            executorData.extraEntity = field;
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

            void bind() {
                delete executor;
            }

            template<typename K, typename ...T>
            void bind(K& entity, T&... entities) {
                Q_ASSERT_X(executor->tbOrderList.at(bindCount) == entity.getTableName(), "dao::DaoJoinExecutorItem::bind", u8"bind实例顺序与查询表顺序不一致");
                auto jp = executor->joinParameters.at(bindCount++);
                for (const auto& p : jp) {
                    entity.bindValue(p, data.at(setCount++));
                }
                bind(entities...);
            }
        };

        friend class DaoJoinExecutorItem;

    public:
        DaoJoinExecutor(const QList<QStringList>& jps, const QString sql, const QStringList tbOrder, const QVariantList& valueList);
        DaoJoinExecutor(const DaoJoinExecutor& executor);

        QList<DaoJoinExecutorItem> list();
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
        template<typename T>
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

        template<typename T>
        SqlJoinBuilder& jleft() {
            return join<T>(JOIN_LEFT);
        }

        template<typename T>
        SqlJoinBuilder& jright() {
            return join<T>(JOIN_RIGHT);
        }

        template<typename T>
        SqlJoinBuilder& jinner() {
            return join<T>(JOIN_INNER);
        }

        template<typename T>
        SqlJoinBuilder& jfull() {
            return join<T>(JOIN_FULL_OUTER);
        }

        template<typename T>
        SqlJoinBuilder& jnull() {
            return join<T>(JOIN_NULL);
        }

        DaoJoinExecutor build();
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
    static void printLog(const QString& lastErr, const QString& sql);
};

class DbLoader {
private:
    class SqlClient {
    public:
        virtual QString findTableStatement(const QString& tbName) = 0;

        template<typename T>
        QString createTableStatement(T& entity);

        template<typename T>
        QString createIndexStatement(T& entity);

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
    static bool isSqlite() {
        return sqlCfg.dbType == "QSQLITE";
    }
    static bool isMysql() {
        return sqlCfg.dbType == "QMYSQL";
    }
    static bool isSqlServer() {
        return sqlCfg.dbType == "QODBC";
    }

    static const SqlCfg& getConfigure() {
        return sqlCfg;
    }
    static SqlClient* getClient() {
        return sqlClient;
    }

    static QVariant readDbSetting(const QString& key, const QString& default);
    static void writeDbSetting(const QString& key, const QVariant& value);

public:
    static void loadConfigure(const QString& fileName);
    static void loadHost(const QString& host) {
        sqlCfg.dbHost = host;
    }
};

class DbCreatorHelper {
public:
    static bool testConnect();
    static bool checkDbVersion();
    static void resetDbVersion();
    static bool checkTableExist(const QString& tbName);

    template<typename T>
    static bool createTable(T entity);

    template<typename T>
    static bool restoreData2NewTable(const QString& tmpTb, T entity);

    static bool renameTable(const QString& oldTb, const QString& newTb);
    static bool dropTable(const QString& tb);
    static bool truncateTable(const QString& tb);

private:
    template<typename F>
    static void execSql(const QString& sql, F fail) {
        execSql(sql, [](auto& query) {}, fail);
    }

    template<typename F1, typename F2>
    static void execSql(const QString& sql, F1 succ, F2 fail);
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
        bool tbRenamed = false;
        QString tmpTbName = entity->getTableName().prepend("tmp_");
        if (!DbCreatorHelper::checkDbVersion() && tbExist) {//表存在且版本不同进行数据库升级
            if (!DbCreatorHelper::renameTable(entity->getTableName(), tmpTbName)) {
                success = false;
                return;
            }
            tbRenamed = true;
        }
        if (!tbExist || tbRenamed) {
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

//**************************************************************************************************************
//********************** DaoExecutor **********************
template<class F>
inline bool dao::DaoExecutor::exec(bool batch, F success) {
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

//********************** DaoCountExecutor **********************
template<typename T>
inline int dao::DaoCountExecutor<T>::count() {
    int count = 0;
    bindTableName();
    exec([&](auto& query) {
        query.next();
        count = query.value(0).toInt();
    });
    return count;
}

//********************** DaoQueryExecutor **********************
template<typename T>
inline T dao::DaoQueryExecutor<T>::unique(bool & exist) {
    Q_ASSERT_X(getWriteEntity().getKvPair().isEmpty(), "dao::_query().unique", u8"不允许使用set函数");
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

template<typename T>
inline QList<T> dao::DaoQueryExecutor<T>::list() {
    Q_ASSERT_X(getWriteEntity().getKvPair().isEmpty(), "dao::_query().list", u8"不允许使用set函数");
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

template<typename T>
inline dao::DaoQueryExecutor<T> & dao::DaoQueryExecutor<T>::mergeQuery(DaoQueryExecutor & other) {
    other.bindTableName();
    other.concatSqlStatement();
    other.mergeValueList();
    tableName = QString("(%1) as res_%2").arg(other.sql_head, other.tableName);
    valueList.append(other.valueList);
    return *this;
}

//********************** DaoInsertExecutor **********************
template<typename T>
inline bool dao::DaoInsertExecutor<T>::insert(T & entity) {
    Q_ASSERT_X(getReadEntity().getKvPair().isEmpty() && getWriteEntity().getKvPair().isEmpty(), "dao::_insert().insert", u8"不允许使用set、wh函数");
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

template<typename T>
inline bool dao::DaoInsertExecutor<T>::insertMutil(const QList<T>& entities) {
    if (entities.isEmpty()) {
        return true;
    }
    Q_ASSERT_X(getReadEntity().getKvPair().isEmpty() && getWriteEntity().getKvPair().isEmpty(), "dao::_insert().insertMutil", u8"不允许使用set、wh函数");
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

template<typename T>
inline bool dao::DaoInsertExecutor<T>::insertBatch(const QList<T>& entities) {
    if (entities.isEmpty()) {
        return true;
    }
    Q_ASSERT_X(getReadEntity().getKvPair().isEmpty() && getWriteEntity().getKvPair().isEmpty(), "dao::_insert().insertBatch", u8"不允许使用set、wh函数");
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

//********************** DaoUpdateExecutor **********************
template<typename T>
inline bool dao::DaoUpdateExecutor<T>::update() {
    Q_ASSERT_X(!getWriteEntity().getKvPair().isEmpty() && !getReadEntity().getKvPair().isEmpty(), "dao::_update().update()", u8"不允许使用set、wh函数");
    bindTableName();
    return exec();
}

template<typename T>
inline bool dao::DaoUpdateExecutor<T>::updateBatch() {
    Q_ASSERT_X(!getWriteEntity().getKvPair().isEmpty() && !getReadEntity().getKvPair().isEmpty(), "dao::_update().updateBatch", u8"不允许使用set、wh函数");
    bindTableName();
    return execBatch();
}

template<typename T>
inline bool dao::DaoUpdateExecutor<T>::update(T & entity) {
    Q_ASSERT_X(getWriteEntity().getKvPair().isEmpty() && !getReadEntity().getKvPair().isEmpty(), "dao::_update().update(T)", u8"不允许使用set、wh函数");

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
    Q_ASSERT_X(findId, "dao::_update().update", u8"查询表没有id字段");
    sql_head.append(setPa.left(setPa.length() - 2));

    return exec();
}

template<typename T>
inline bool dao::DaoUpdateExecutor<T>::updateBy(T & entity) {
    Q_ASSERT_X(!getBindEntities().isEmpty(), "dao::_update().updateBy", u8"不允许使用bind");
    Q_ASSERT_X(getWriteEntity().getKvPair().isEmpty() && getReadEntity().getKvPair().isEmpty(), "dao::_update().updateBy", u8"不允许使用set、wh函数");

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
    Q_ASSERT_X(findId, "dao::_update().updateBy", u8"查询表没有id字段");
    sql_head.append(setPa.left(setPa.length() - 2) + whPa.left(whPa.length() - 6));
    valueList.append(cndVList);
    return exec();
}

template<typename T>
inline bool dao::DaoUpdateExecutor<T>::updateBatch(QList<T>& entities) {
    Q_ASSERT_X(!getBindEntities().isEmpty(), "dao::_update().updateBatch", u8"不允许使用bind");
    Q_ASSERT_X(getWriteEntity().getKvPair().isEmpty() && getReadEntity().getKvPair().isEmpty(), "dao::_update().updateBatch", u8"不允许使用set、wh函数");
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
    Q_ASSERT_X(findId, "dao::_update().updateBatch", u8"查询表没有id字段");

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

//********************** DaoDeleteExecutor **********************
template<typename T>
inline bool dao::DaoDeleteExecutor<T>::deleteBy() {
    bindTableName();
    return exec();
}

template<typename T>
inline bool dao::DaoDeleteExecutor<T>::deleteBatch() {
    bindTableName();
    return execBatch();
}

//********************** SqlBuilder **********************
template<typename E, typename K>
inline K dao::SqlBuilder<E, K>::build() {
    QString sql;
    switch (executorData.operateType) {
        case OPERATE_QUERY:
            if (executorData.bindEntities.isEmpty()) {
                sql = "select *from ";
            } else {
                QString bindEntitiesStr;
                for (auto& bindEntity : executorData.bindEntities) {
                    bindEntitiesStr.append(bindEntity.nameExtra()).append(',');
                    if (!bindEntity.asIsName()) {
                        executorData.bindExtraName.append(bindEntity.asString());
                    }
                }
                bindEntitiesStr = bindEntitiesStr.left(bindEntitiesStr.length() - 1);
                sql = "select " + bindEntitiesStr + " from ";
            }
            break;
        case OPERATE_INSERT:
            sql = "insert into ";
            break;
        case OPERATE_INSERT_OR_REPLACE:
            sql = "replace into ";
            break;
        case OPERATE_UPDATE:
            sql = "update ";
            break;
        case OPERATE_DELETE:
            sql = "delete from ";
            break;
        case OPERATE_COUNT:
            sql = "select count(*) from ";
            break;
        default:
            break;
    }
    return K(sql, static_cast<E*>(0)->getTableName(), &executorData);
}

//********************** SqlClient **********************
template<typename T>
inline QString DbLoader::SqlClient::createTableStatement(T & entity) {
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
inline QString DbLoader::SqlClient::createIndexStatement(T & entity) {
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

//********************** DbCreatorHelper **********************
template<typename T>
inline bool DbCreatorHelper::createTable(T entity) {
    bool success = true;
    execSql(DbLoader::getClient()->createTableStatement(entity), [&](auto& lastErr) {
        qDebug() << "create table fail, err = " << lastErr;
        success = false;
        Q_ASSERT_X(success, "DbCreatorHelper::createTable", lastErr.text().toLatin1());
    });

    if (success) {
        if (!entity->getIndexFields().isEmpty()) {
            execSql(DbLoader::getClient()->createIndexStatement(entity), [&](auto& lastErr) {
                qDebug() << "create index fail, err = " << lastErr;
                if (lastErr.nativeErrorCode() != "1061") {
                    success = false;
                }
                Q_ASSERT_X(success, "DbCreatorHelper::createTable", lastErr.text().toLatin1());
            });
        }
    }
    return success;
}

template<typename T>
inline bool DbCreatorHelper::restoreData2NewTable(const QString & tmpTb, T entity) {
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

template<typename F1, typename F2>
inline void DbCreatorHelper::execSql(const QString & sql, F1 succ, F2 fail) {
    auto db = ConnectionPool::openConnection();
    QSqlQuery query(db);
    if (query.exec(sql)) {
        succ(query);
    } else {
        fail(query.lastError());
    }
    ConnectionPool::closeConnection(db);
}

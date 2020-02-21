#pragma once

#include "ConnectionPool.h"
#include <qdebug.h>
#include <qfile.h>
#include <qerrormessage.h>
#include "EntityField.h"
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
        //operate (bindCondition|*) (into|from) (bindTableName) (set setCondition) (where whereCondition subWhereCondition)
        OperateType operateType;
        EntityConditions bindCondition, setCondition, whereCondition, subWhereCondition;
        bool fromExtra;//from嵌套
        QString bindTableName;
        QVariantList bindTableNameContainValues;//from嵌套查询中的值列表
        QList<EntityConditions> bindTableNameJoinEntityGroup;//from-join嵌套查询中的join-bind字段列表
    };

    template<typename K> class SqlBuilder;
    class DaoExecutor {
    public:
        DaoExecutor(ExecutorData* executorData);

    protected:
        /*executor prepare*/
        void createSqlHead();
        void concatSqlStatement();
        void mergeValueList();
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

    protected:
        ExecutorData* executorData;
        QString sqlExpression;
        QVariantList valueList;

    private:
        template<typename F>
        bool exec(bool batch, F success);

        virtual QString getTableName() = 0;

    protected:
        template<typename T>
        QString getTableNameFromTemplate() {
            if (executorData->bindTableName.isEmpty()) {
                return static_cast<T*>(0)->getTableName();
            } else {
                return '(' + executorData->bindTableName + ") as res_" + static_cast<T*>(0)->getTableName();
            }
        }

        template<typename K>
        friend class SqlBuilder;
    };

    template<typename E, typename ExecutorNext>
    class MutilTableNameCreator : public ExecutorNext {
    public:
        using ExecutorNext::ExecutorNext;
        
    public:
        QString getTableName() override {
            if (executorData->bindTableName.isEmpty()) {
                auto index = tableOrder++;
                auto subTbName = ExecutorNext::getTableName();
                if (subTbName.isEmpty()) {
                    return getTableNameFromTemplate<E>() + ' ' + ('a' + index);
                }
                return getTableNameFromTemplate<E>() + ' ' + ('a' + index) + ',' + subTbName;
            } else {
                return '(' + executorData->bindTableName + ") as res_" + static_cast<E*>(0)->getTableName();
            }
        }
    };

    class MutilTableNameCreatorEnd : public DaoExecutor {
    public:
        MutilTableNameCreatorEnd(ExecutorData* executorData) : DaoExecutor(executorData) {
            tableOrder = 0;
        };

    protected:
        QString getTableName() {
            return QString();
        }

        int tableOrder;
    };

    template<typename...T> class DaoCountExecutor;
    template<typename E, typename...T>
    class DaoCountExecutor<E, T...> : public MutilTableNameCreator<E, DaoCountExecutor<T...>> {
    public:
        using MutilTableNameCreator<E, DaoCountExecutor<T...>>::MutilTableNameCreator;

        int count();
    };

    template<> class DaoCountExecutor<> : public MutilTableNameCreatorEnd {
    public:
        using MutilTableNameCreatorEnd::MutilTableNameCreatorEnd;
    };

    template<typename T>
    class DaoQueryExecutor : public DaoExecutor {
    public:
        using DaoExecutor::DaoExecutor;

        T unique(bool& exist);

        QList<T> list();

    private:
        QString getTableName() override {
            return getTableNameFromTemplate<T>();
        }
    };

    template<typename...T> class DaoQueryMutilExecutor;
    template<typename E, typename...T>
    class DaoQueryMutilExecutor<E, T...> : public MutilTableNameCreator<E, DaoQueryMutilExecutor<T...>> {
    public:
        using MutilTableNameCreator<E, DaoQueryMutilExecutor<T...>>::MutilTableNameCreator;

        void unique(bool& exist, E& entity, T&... t);
        void list(QList<E>& entityList, QList<T>&... t);

    protected:
        void getFieldsIndex(int index, QVector<QList<int>>& fieldsIndex);
        void valueInsert(QSqlRecord & record, QVector<QList<int>>& fieldsIndex, E& entity, T&... t);
        void valueInsert(QSqlRecord & record, QVector<QList<int>>& fieldsIndex, QList<E>& entityList, QList<T>&... t);

    private:
        QVector<QList<int>> getBindEntityIndex();
    };

    template<> class DaoQueryMutilExecutor<> : public MutilTableNameCreatorEnd {
    public:
        using MutilTableNameCreatorEnd::MutilTableNameCreatorEnd;

    protected:
        void getFieldsIndex(int index, QVector<QList<int>>& fields) {
        }
        void valueInsert(QSqlRecord & record, QVector<QList<int>>& fieldsIndex) {
        }
    };

    template<typename T>
    class DaoInsertExecutor : public DaoExecutor {
    public:
        using DaoExecutor::DaoExecutor;

        bool insert(T& entity);
        bool insertMutil(const QList<T>& entities);
        bool insertBatch(const QList<T>& entities);

    private:
        QString getTableName() override {
            return getTableNameFromTemplate<T>();
        }
    };

    template<typename T>
    class DaoUpdateExecutor : public DaoExecutor {
    public:
        using DaoExecutor::DaoExecutor;

        /*used by 'set' and 'where' conditions to update table */
        bool update();
        /*used by 'set' and 'where' conditions to update table, value list must be QVariantList */
        bool updateBatch();
        /*used by 'id' to update entity to table*/
        bool update(T& entity);
        /*used by 'id' to update entities to table*/
        bool updateBatch(QList<T>& entities);

    private:
        QString getTableName() override {
            return getTableNameFromTemplate<T>();
        }
    };

    template<typename T>
    class DaoDeleteExecutor : public DaoExecutor {
    public:
        using DaoExecutor::DaoExecutor;

        bool deleteBy();
        bool deleteBatch();

    private:
        QString getTableName() override {
            return getTableNameFromTemplate<T>();
        }
    };

    class DaoJoinExecutor;

    template<typename K>
    class SqlBuilder {
    private:
        ExecutorData executorData;

    public:
        SqlBuilder(OperateType type) {
            executorData.operateType = type;
            executorData.fromExtra = false;
        }

        SqlBuilder& bind() {
            return *this;
        }
        /*bind multiply fields*/
        template<typename F, typename ...T>
        SqlBuilder& bind(const F& f, const T&... t) {
            (executorData.bindCondition, f);
            return bind(t...);
        }

        K build();

        SqlBuilder& wh(const EntityConditions& condition) {
            executorData.whereCondition = condition;
            return *this;
        }

        SqlBuilder& set() {
            return *this;
        }

        template<typename F, typename ...T>
        SqlBuilder& set(const F& f, const T&... t) {
            (executorData.setCondition, f);
            return set(t...);
        }

        SqlBuilder& subWh() {
            return *this;
        }

        template<typename F, typename ...T>
        SqlBuilder& subWh(const F& f, const T&... t) {
            (executorData.subWhereCondition, f);
            return subWh(t...);
        }

        /*used by nested query*/
        SqlBuilder& from(DaoExecutor& executor) {
            executor.createSqlHead();
            executor.concatSqlStatement();
            executorData.bindTableName = executor.sqlExpression;
            executor.mergeValueList();
            executorData.bindTableNameContainValues = executor.valueList;
            executorData.fromExtra = true;
            return *this;
        }

        /*used by nested query*/
        SqlBuilder& from(DaoJoinExecutor& executor) {
            executorData.bindTableName = executor.sqlExpression;
            executorData.bindTableNameContainValues = executor.valueList;
            for (int i = 0; i < executor.joinInfo.size(); i++) {
                executorData.bindTableNameJoinEntityGroup.append(executor.joinInfo.at(i).bindCondition);
            }
            executorData.fromExtra = true;
            return *this;
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
            EntityConditions bindCondition, whereCondition, subWhCondition;
        };
        QList<JoinInfo> joinInfos;
        JoinInfo tmpjoinInfo;

        friend class DaoJoinExecutor;

    private:
        template<typename T>
        SqlJoinBuilder& join(JoinType joinType) {
            tmpjoinInfo.tbName = static_cast<T*>(0)->getTableName();
            tmpjoinInfo.joinType = joinType;
            joinInfos.append(tmpjoinInfo);
            tmpjoinInfo.bindCondition.clearAll();
            tmpjoinInfo.whereCondition.clearAll();
            tmpjoinInfo.subWhCondition.clearAll();
            return *this;
        }

    public:
        SqlJoinBuilder& bind() {
            return *this;
        }

        template<typename F, typename ...T>
        SqlJoinBuilder& bind(const F& f, const T&... t) {
            (tmpjoinInfo.bindCondition, f);
            return bind(t...);
        }

        SqlJoinBuilder& wh(const EntityConditions& condition) {
            tmpjoinInfo.whereCondition = condition;
            return *this;
        }

        SqlJoinBuilder& subWh() {
            return *this;
        }

        template<typename F, typename ...T>
        SqlJoinBuilder& subWh(const F& f, const T&... t) {
            (tmpjoinInfo.subWhCondition, f);
            return subWh(t...);
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

    class DaoJoinExecutor {
    private:
        QList<SqlJoinBuilder::JoinInfo> joinInfo;

        QString sqlExpression;
        QVariantList valueList;

        class DaoJoinExecutorItemWarpper;
        class DaoJoinExecutorItem {
        private:
            const QList<SqlJoinBuilder::JoinInfo>* joinInfo;
            QVariantList data;
            int bindCount = 0;
            int setCount = 0;

            friend class DaoJoinExecutorItemWarpper;

        public:
            DaoJoinExecutorItem(const QVariantList& data, const QList<SqlJoinBuilder::JoinInfo>* joinInfo) {
                this->joinInfo = joinInfo;
                this->data = data;
            }

            void bind() {
            }

            template<typename K, typename ...T>
            void bind(K& entity, T&... entities) {
                Q_ASSERT_X(joinInfo->at(bindCount).tbName == entity.getTableName(), "dao::DaoJoinExecutorItem::bind", u8"bind实例顺序与查询表顺序不一致");
                auto bindFields = joinInfo->at(bindCount).bindCondition.getBindFields(true);
                for (const auto& fieldStr : bindFields) {
                    entity.bindValue(fieldStr, data.at(setCount++));
                }
                bindCount++;
                bind(entities...);
            }
        };

        class DaoJoinExecutorItemWarpper {
        private:
            QList<DaoJoinExecutorItem> items;
            QList<SqlJoinBuilder::JoinInfo> *joinInfo;
            bool infoAttach = false;
            friend class DaoJoinExecutor;

        public:
            DaoJoinExecutorItemWarpper(const QList<SqlJoinBuilder::JoinInfo>& joinInfo) {
                this->joinInfo = new QList<SqlJoinBuilder::JoinInfo>(joinInfo);
                infoAttach = true;
            }

            DaoJoinExecutorItemWarpper(const DaoJoinExecutorItemWarpper& wrapper) {
                *this = wrapper;
                infoAttach = false;
            }

            QList<DaoJoinExecutorItem>& list() {
                return items;
            }

            void append(DaoJoinExecutorItem& item) {
                items << item;
            }

            inline QList<DaoJoinExecutorItem>::iterator begin() {
                return items.begin();
            }

            inline QList<DaoJoinExecutorItem>::iterator end() {
                return items.end();
            }

            ~DaoJoinExecutorItemWarpper() {
                if (!infoAttach) {
                    delete joinInfo;
                }
            }
        };

        friend class DaoJoinExecutorItem;
        friend class SqlJoinBuilder;
        template<typename K>
        friend class SqlBuilder;

        DaoJoinExecutor(QList<SqlJoinBuilder::JoinInfo> joinInfo, const QString sql, const QVariantList& valueList);
        
    public:
        DaoJoinExecutorItemWarpper list();
    };

public:
    template<typename E>
    static SqlBuilder<DaoQueryExecutor<E>> _query() {
        return SqlBuilder<DaoQueryExecutor<E>>(OPERATE_QUERY);
    }

    template<typename...E>
    static SqlBuilder<DaoQueryMutilExecutor<E...>> _query_mutil() {
        return SqlBuilder<DaoQueryMutilExecutor<E...>>(OPERATE_QUERY);
    }

    template<typename E>
    static SqlBuilder<DaoInsertExecutor<E>> _insert() {
        return SqlBuilder<DaoInsertExecutor<E>>(OPERATE_INSERT);
    }

    template<typename E>
    static SqlBuilder<DaoInsertExecutor<E>> _replace() {
        return SqlBuilder<DaoInsertExecutor<E>>(OPERATE_INSERT_OR_REPLACE);
    }

    template<typename E>
    static SqlBuilder<DaoUpdateExecutor<E>> _update() {
        return SqlBuilder<DaoUpdateExecutor<E>>(OPERATE_UPDATE);
    }

    template<typename E>
    static SqlBuilder<DaoDeleteExecutor<E>> _delete() {
        return SqlBuilder<DaoDeleteExecutor<E>>(OPERATE_DELETE);
    }

    template<typename...E>
    static SqlBuilder<DaoCountExecutor<E...>> _count() {
        return SqlBuilder<DaoCountExecutor<E...>>(OPERATE_COUNT);
    }

    static SqlJoinBuilder _join() {
        return SqlJoinBuilder();
    }

    static void bindTableOrder() {
        bindCount = 0;
    }

    template<typename K, typename... T>
    static void bindTableOrder(K& k, T&... t) {
        k.bindJoin(bindCount++);
        bindTableOrder(t...);
    }

    static void clearTableOrder() {
    }

    template<typename K, typename... T>
    static void clearTableOrder(K& k, T&... t) {
        k.clearJoin();
        clearTableOrder(t...);
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

//execute sql expression
template<typename F>
inline bool dao::DaoExecutor::exec(bool batch, F success) {
    auto db = ConnectionPool::openConnection();
    QSqlQuery query(db);
    query.prepare(sqlExpression);
    bindValue(query);
#ifdef PRINT_SQL_STATEMENT
    qDebug() << sqlExpression;
#endif // PRINT_SQL_STATEMENT
    bool ok = batch ? query.execBatch() : query.exec();
    if (!ok) {
        dao::printLog(query.lastError().text(), sqlExpression);
    } else {
        success(query);
    }
    ConnectionPool::closeConnection(db);
    return ok;
}

//********************** DaoCountExecutor **********************

template<typename E, typename ...T>
inline int dao::DaoCountExecutor<E, T...>::count() {
    int count = 0;
    createSqlHead();
    concatSqlStatement();
    exec([&](auto& query) {
        query.next();
        count = query.value(0).toInt();
    });
    return count;
}

//********************** DaoQueryExecutor **********************

template<typename T>
inline T dao::DaoQueryExecutor<T>::unique(bool & exist) {
    T entity;
    exist = false;
    createSqlHead();
    concatSqlStatement();
    exec([&](auto& query) {
        if (query.next()) {
            exist = true;
            QSqlRecord record = query.record();
            if (executorData->bindCondition.getExpressionStr().isEmpty()) {
                for (int i = 0; i < record.count(); i++) {
                    entity.bindValue(record.fieldName(i), record.value(i));
                }
            } else {
                auto bindFields = executorData->bindCondition.getBindFields();
                for (const auto& f : bindFields) {
                    entity.bindValue(f, record.value(f));
                }
            }
        }
    });
    return entity;
}

template<typename T>
inline QList<T> dao::DaoQueryExecutor<T>::list() {
    QList<T> entities;
    T entity;
    createSqlHead();
    concatSqlStatement();
    exec([&](auto& query) {
        while (query.next()) {
            QSqlRecord record = query.record();
            if (executorData->bindCondition.getExpressionStr().isEmpty()) {
                for (int i = 0; i < record.count(); i++) {
                    entity.bindValue(record.fieldName(i), record.value(i));
                }
            } else {
                auto bindFields = executorData->bindCondition.getBindFields();
                for (const auto& f : bindFields) {
                    entity.bindValue(f, record.value(f));
                }
            }
            entities.append(entity);
        }
    });
    return entities;
}

template<typename E, typename ...T>
inline void dao::DaoQueryMutilExecutor<E, T...>::unique(bool & exist, E & entity, T & ...t) {
    exist = false;
    createSqlHead();
    concatSqlStatement();
    exec([&](auto& query) {
        if (query.next()) {
            exist = true;
            valueInsert(query.record(), getBindEntityIndex(), entity, t...);
        }
    });
}

template<typename E, typename ...T>
inline void dao::DaoQueryMutilExecutor<E, T...>::list(QList<E>& entityList, QList<T>& ...t) {
    createSqlHead();
    concatSqlStatement();
    exec([&](auto& query) {
        QVector<QList<int>> bindEntitiesIndex = getBindEntityIndex();
        while (query.next()) {
            valueInsert(query.record(), bindEntitiesIndex, entityList, t...);
        }
    });
}

template<typename E, typename ...T>
inline void dao::DaoQueryMutilExecutor<E, T...>::getFieldsIndex(int index, QVector<QList<int>>& fieldsIndex) {
    int tableIndex = fieldsIndex.size() - (sizeof...(T) + 1);
    int fieldSize = static_cast<E*>(0)->fieldSize();
    for (int i = 0; i < fieldSize; i++) {
        fieldsIndex[tableIndex] << index++;
    }
    dao::DaoQueryMutilExecutor<T...>::getFieldsIndex(index, fieldsIndex);
}

template<typename E, typename ...T>
inline void dao::DaoQueryMutilExecutor<E, T...>::valueInsert(QSqlRecord & record, QVector<QList<int>>& fieldsIndex, E& entity, T&... t) {
    int tableIndex = fieldsIndex.size() - (sizeof...(T) + 1);
    const auto& indexes = fieldsIndex[tableIndex];
    for (int i = 0; i < indexes.size(); i++) {
        int valueIndex = indexes.at(i);
        entity.bindValue(record.fieldName(valueIndex), record.value(valueIndex));
    }
    dao::DaoQueryMutilExecutor<T...>::valueInsert(record, fieldsIndex, t...);
}

template<typename E, typename ...T>
inline void dao::DaoQueryMutilExecutor<E, T...>::valueInsert(QSqlRecord & record, QVector<QList<int>>& fieldsIndex, QList<E>& entityList, QList<T>& ...t) {
    int tableIndex = fieldsIndex.size() - (sizeof...(T) + 1);
    const auto& indexes = fieldsIndex[tableIndex];
    E entity;
    for (int i = 0; i < indexes.size(); i++) {
        int valueIndex = indexes.at(i);
        entity.bindValue(record.fieldName(valueIndex), record.value(valueIndex));
    }
    entityList << entity;
    dao::DaoQueryMutilExecutor<T...>::valueInsert(record, fieldsIndex, t...);
}

template<typename E, typename ...T>
inline QVector<QList<int>> dao::DaoQueryMutilExecutor<E, T...>::getBindEntityIndex() {
    QVector<QList<int>> bindEntitiesIndex;
    bindEntitiesIndex.resize(sizeof...(T) + 1);
    if (!executorData->fromExtra) {
        if (executorData->bindCondition.getExpressionStr().isEmpty()) {
            getFieldsIndex(0, bindEntitiesIndex);
        } else {
            const QList<EntityField>& entityFields = executorData->bindCondition.getBindEntities();
            int valueIndex = 0;
            for (const auto& field : entityFields) {
                if (!field.fieldWithoutJoinPredix().isEmpty() || field.isFuntion()) {
                    bindEntitiesIndex[field.getBindOrderIndex()] << valueIndex++;
                }
            }
        }
    } else {
        int valueIndex = 0;
        QList<EntityConditions> bindTableNameJoinEntityGroup = executorData->bindTableNameJoinEntityGroup;
        if (executorData->bindCondition.getExpressionStr().isEmpty()) {
            for (int i = 0; i < bindTableNameJoinEntityGroup.size(); i++) {
                auto condition = bindTableNameJoinEntityGroup.at(i);
                condition.clearCombineOp();
                const QList<EntityField>& entityFields = condition.getBindEntities();
                for (const auto& field : entityFields) {
                    if (!field.fieldWithoutJoinPredix().isEmpty() || field.isFuntion()) {
                        bindEntitiesIndex[field.getBindOrderIndex()] << valueIndex++;
                    }
                }
            }
        } else {
            QStringList entityFieldsStr = executorData->bindCondition.getBindFields(true);
            for (const auto& fieldName : entityFieldsStr) {
                bool getIndex = false;
                for (int i = 0; i < bindTableNameJoinEntityGroup.size(); i++) {
                    int col = 0;
                    bindTableNameJoinEntityGroup[i].clearCombineOp();
                    QList<EntityField>& conditionFields = bindTableNameJoinEntityGroup[i].getBindEntities();
                    while (col != conditionFields.size()) {
                        EntityField& subField = conditionFields[col];
                        bool findSub = false;
                        if (subField.isFuntion()) {
                            findSub = subField.funtionAsField() == fieldName;
                        } else {
                            findSub = subField.fieldWithoutJoinPredix() == fieldName;
                        }
                        if (findSub) {
                            bindEntitiesIndex[subField.getBindOrderIndex()] << valueIndex++;
                            getIndex = true;
                            conditionFields.removeAt(col);
                            break;
                        }
                        col++;
                    }
                    if (col != conditionFields.size()) {
                        break;
                    }
                }
                if (!getIndex) {
                    bindEntitiesIndex[0] << valueIndex++;
                }
            }
        }
    }
    return bindEntitiesIndex;
}


//********************** DaoInsertExecutor **********************

template<typename T>
inline bool dao::DaoInsertExecutor<T>::insert(T & entity) {
    createSqlHead();

    QString valueStr = " values(%1)";
    QVariantList entityData = entity.readEntity();
    QString prepareStr = QString("?,").repeated(entityData.size());
    sqlExpression.append(valueStr.arg(prepareStr.left(prepareStr.length() - 1)));

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
    createSqlHead();

    sqlExpression.append(" values");
    int fieldSize = static_cast<T*>(0)->fieldSize();
    QString prepareStr = QString("?,").repeated(fieldSize);
    prepareStr = prepareStr.left(prepareStr.length() - 1);
    for (int i = 0; i < entities.size(); i++) {
        sqlExpression.append(QString("(").append(prepareStr).append("),"));
    }
    sqlExpression = sqlExpression.left(sqlExpression.length() - 1);
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
    createSqlHead();

    QString valueStr = " values(%1)";
    int fieldSize = static_cast<T*>(0)->fieldSize();
    QString prepareStr = QString("?,").repeated(fieldSize);
    sqlExpression.append(valueStr.arg(prepareStr.left(prepareStr.length() - 1)));

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
    createSqlHead();
    concatSqlStatement();
    return exec();
}

template<typename T>
inline bool dao::DaoUpdateExecutor<T>::updateBatch() {
    createSqlHead();
    concatSqlStatement();
    return execBatch();
}

template<typename T>
inline bool dao::DaoUpdateExecutor<T>::update(T & entity) {
    Q_ASSERT_X(executorData->whereCondition.getExpressionStr().isEmpty(),
               "dao::_update().update(T&)", u8"不允许使用where条件");
    createSqlHead();

    auto idField = static_cast<T*>(0)->getIdField();

    QVariantList entityData = entity.readEntity();
    QStringList fields = static_cast<T*>(0)->getFields();
    int col = 0;
    while (col != fields.size()) {
        if (idField == fields.at(col)) {
            fields.takeAt(col); 
            executorData->whereCondition = EntityField(idField) == entity.getId();
            entityData.takeAt(col);
            break;
        }
        col++;
    }
    EntityConditions conditions;
    for (int i = 0; i < fields.size(); i++) {
        auto field = fields.at(i);
        if (field.isEmpty())
            continue;
        (conditions, EntityField(field) == entityData.at(i));
    }
    executorData->setCondition = conditions;
    concatSqlStatement();

    return exec();
}

template<typename T>
inline bool dao::DaoUpdateExecutor<T>::updateBatch(QList<T>& entities) {
    if (entities.isEmpty())
        return true;

    Q_ASSERT_X(executorData->whereCondition.getExpressionStr().isEmpty(),
               "dao::_update().updateBatch(QList<T>&)", u8"不允许使用where条件");

    createSqlHead();
    QVector<QVariantList> entityDataList;

    QStringList fields = static_cast<T*>(0)->getFields();
    entityDataList.resize(fields.size());
    for (const auto& entity : entities) {
        QVariantList entityData = entity.readEntity();
        for (int i = 0; i < fields.size(); i++) {
            entityDataList[i].append(entityData.at(i));
        }
    }

    auto idField = static_cast<T*>(0)->getIdField();

    int col = 0;
    QVariantList idList;
    while (col != fields.size()) {
        if (idField == fields.at(col)) {
            fields.takeAt(col);
            for (const auto& entity : entities) {
                idList << entity.getId();
            }
            entityDataList.takeAt(col);
            break;
        }
        col++;
    }
    executorData->whereCondition = EntityField(idField) == idList;
    EntityConditions conditions;
    for (int i = 0; i < fields.size(); i++) {
        auto field = fields.at(i);
        if (field.isEmpty())
            continue;
        (conditions, EntityField(field) == entityDataList.at(i));
    }

    executorData->setCondition = conditions;
    concatSqlStatement();

    return execBatch();
}

//********************** DaoDeleteExecutor **********************
template<typename T>
inline bool dao::DaoDeleteExecutor<T>::deleteBy() {
    createSqlHead();
    concatSqlStatement();
    return exec();
}

template<typename T>
inline bool dao::DaoDeleteExecutor<T>::deleteBatch() {
    createSqlHead();
    concatSqlStatement();
    return execBatch();
}

//********************** SqlBuilder **********************
template<typename K>
inline K dao::SqlBuilder<K>::build() {
    return K(&executorData); 
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
            qDebug() << "restore tmp data fail!" << sql << "  err ->" << query.lastError();
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



#pragma once

#include "BaseQuery.h"

#include "../macro/macro.h"

template<typename E>
class DeleteBuilder;

template<typename E>
class Delete : BaseQuery {
public:
    /// <summary>
    /// delete by filter condition
    /// </summary>
    /// <returns></returns>
    bool deleteBy();

    /// <summary>
    /// delete by filter condition in batches
    /// </summary>
    /// <returns></returns>
    bool deleteBatch();

    /// <summary>
    /// delete object by primary key
    /// </summary>
    /// <param name="entity"></param>
    /// <returns></returns>
    bool deleteBy(const E& entity);

    /// <summary>
    /// delete object by primary key in batches
    /// </summary>
    /// <param name="entities"></param>
    /// <returns></returns>
    bool deleteBatch(const QList<E>& entities);

private:
    void buildDeleteByFilterSqlStatement();
    void buildDeleteEntitiesCondition(const QList<E>& entities);

private:
    BASE_QUERY_CONSTRUCTOR_DECLARE_W(Delete)
};

template<typename E>
inline bool Delete<E>::deleteBy() {
    buildDeleteByFilterSqlStatement();
    bool deleteSuccess = false;
    exec([&](QSqlQuery& query) {
        Q_UNUSED(query);
        deleteSuccess = true;
    });
    return deleteSuccess;
}

template<typename E>
inline bool Delete<E>::deleteBatch() {
    buildDeleteByFilterSqlStatement();
    bool deleteSuccess = false;
    execBatch([&](QSqlQuery& query) {
        Q_UNUSED(query);
        deleteSuccess = true;
    });
    return deleteSuccess;
}

template<typename E>
inline bool Delete<E>::deleteBy(const E& entity) {
    buildDeleteEntitiesCondition(QList<E>() << entity);
    return deleteBy();
}

template<typename E>
inline bool Delete<E>::deleteBatch(const QList<E>& entities) {
    buildDeleteEntitiesCondition(entities);
    return deleteBatch();
}

template<typename E>
inline void Delete<E>::buildDeleteByFilterSqlStatement() {
    typename E::Info info;
    QString sql = "delete from ";
    sql.append(info.getTableName());

    QVariantList value;
    if (!builder->filterCondition.isEmpty()) {
        builder->filterCondition.connect();
        sql.append(" where ").append(builder->filterCondition.getConditionStr());
        value << builder->filterCondition.getValues();
    }
    setSqlQueryStatement(sql, value);
}

template<typename E>
inline void Delete<E>::buildDeleteEntitiesCondition(const QList<E>& entities) {
    typename E::Info info;
    typename E::Tool tool;
    QStringList primaryKeys = info.getPrimaryKeys();
    Q_ASSERT(!primaryKeys.isEmpty());

    for (const auto& field : primaryKeys) {
        QVariantList fieldValue;
        for (const auto& entity : entities) {
            fieldValue << tool.getValueByName(entity, field);
        }
        auto condition = 
            EntityCondition(FieldInfo{ field, info.getTableName() }, "=", fieldValue.size() == 1 ? fieldValue.at(0) : fieldValue);
        builder->filterCondition.append(condition);
    }
}

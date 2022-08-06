#pragma once

#include "basequery.h"

#include "../macro/macro.h"

template<typename E>
class UpdateBuilder;

template<typename E>
class Update : BaseQuery {
public:
    /// <summary>
    /// using the SET condition to update a record
    /// </summary>
    /// <returns></returns>
    bool update();

    /// <summary>
    /// using the SET condition to update a record in batches 
    /// </summary>
    /// <returns></returns>
    bool updateBatch();

    /// <summary>
    /// update by object's primary key
    /// </summary>
    /// <param name="entity"></param>
    /// <returns></returns>
    bool update(const E& entity);

    /// <summary>
    /// update by object's primary key in batches 
    /// </summary>
    /// <param name="entities"></param>
    /// <returns></returns>
    bool updateBatch(const QList<E>& entities);

private:
    void buildUpdateBySetSqlStatement();
    void bindUpdateEntitiesCondition(const QList<E>& entities);

private:
    BASE_QUERY_CONSTRUCTOR_DECLARE_W(Update)
};

template<typename E>
inline bool Update<E>::update() {
    buildUpdateBySetSqlStatement();
    bool updateSuccess = false;
    exec([&](QSqlQuery& query) {
        updateSuccess = true;
        int affectRow = query.numRowsAffected();
        if (affectRow <= 0) {
            printWarning("update last effect row is 0!");
        }
    });
    return updateSuccess;
}

template<typename E>
inline bool Update<E>::updateBatch() {
    buildUpdateBySetSqlStatement();
    bool updateSuccess = false;
    execBatch([&](QSqlQuery& query) {
        updateSuccess = true;
        int affectRow = query.numRowsAffected();
        if (affectRow <= 0) {
            printWarning("update batch last effect row is 0!");
        }
    });
    return updateSuccess;
}

template<typename E>
inline bool Update<E>::update(const E& entity) {
    bindUpdateEntitiesCondition(QList<E>() << entity);
    return update();
}

template<typename E>
inline bool Update<E>::updateBatch(const QList<E>& entities) {
    bindUpdateEntitiesCondition(entities);
    return updateBatch();
}

template<typename E>
inline void Update<E>::buildUpdateBySetSqlStatement() {
    typename E::Info info;
    QString sql = "update ";
    sql.append(info.getTableName());

    QVariantList value;

    builder->setCondition.connect();
    Q_ASSERT(!builder->setCondition.getConditionStr().isEmpty());
    sql.append(" set ");
    sql.append(builder->setCondition.getConditionStr());
    value << builder->setCondition.getValues();

    if (!builder->filterCondition.isEmpty()) {
        builder->filterCondition.connect();
        sql.append(" where ").append(builder->filterCondition.getConditionStr());
        value << builder->filterCondition.getValues();
    }
    setSqlQueryStatement(sql, value);
}

template<typename E>
inline void Update<E>::bindUpdateEntitiesCondition(const QList<E>& entities) {
    Q_ASSERT(builder->setCondition.isEmpty());
    Q_ASSERT(builder->filterCondition.isEmpty());
    typename E::Info info;
    typename E::Tool tool;
    QStringList primaryKeys = info.getPrimaryKeys();
    Q_ASSERT(!primaryKeys.isEmpty());
    QStringList fields = info.getFields();
    for (const auto& field : fields) {
        QVariantList fieldValue;
        for (const auto& entity : entities) {
            fieldValue << tool.getValueByName(entity, field);
        }
        auto condition = 
            EntityCondition(FieldInfo{ field, info.getTableName() }, "=", fieldValue.size() == 1 ? fieldValue.at(0) : fieldValue);
        if (primaryKeys.contains(field)) {
            builder->filterCondition.append(condition);
        } else {
            builder->setCondition.append(condition);
        }
    }
}

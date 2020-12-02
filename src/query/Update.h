#pragma once

#include "BaseQuery.h"

template<typename T>
class UpdateBuilder;

template<typename E>
class Update : BaseQuery {
public:
    /// <summary>
    /// 通过set条件更新
    /// </summary>
    /// <returns></returns>
    bool update();

    /// <summary>
    /// 通过set条件批量更新
    /// </summary>
    /// <returns></returns>
    bool updateBatch();

    /// <summary>
    /// 通过实例primary key更新
    /// </summary>
    /// <param name="entity"></param>
    /// <returns></returns>
    bool update(const E& entity);

    /// <summary>
    /// 通过实例primary key批量更新
    /// </summary>
    /// <param name="entities"></param>
    /// <returns></returns>
    bool updateBatch(const QList<E>& entities);

private:
    void buildUpdateBySetSqlStatement();
    void bindUpdateEntitiesCondition(const QList<E>& entities);

private:
    Connector setCondition, filterCondition;

    friend class UpdateBuilder<E>;
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

    setCondition.connect();
    Q_ASSERT(!setCondition.getConditionStr().isEmpty());
    sql.append(" set ");
    sql.append(setCondition.getConditionStr());
    value << setCondition.getValues();

    filterCondition.connect();
    if (!filterCondition.getConditionStr().isEmpty()) {
        sql.append(" where ").append(filterCondition.getConditionStr());
        value << filterCondition.getValues();
    }
    setSqlQueryStatement(sql, value);
}

template<typename E>
inline void Update<E>::bindUpdateEntitiesCondition(const QList<E>& entities) {
    Q_ASSERT(setCondition.isEmpty());
    Q_ASSERT(filterCondition.isEmpty());
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
            filterCondition.append(condition);
        } else {
            setCondition.append(condition);
        }
    }
}

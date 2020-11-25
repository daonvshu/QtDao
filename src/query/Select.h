#pragma once

#include "BaseQuery.h"

template<typename T>
class SelectBuilder;

template<typename E>
class Select : public BaseQuery {
public:
    E unique();

    E one();

    QList<E> list();

private:
    void buildFilterSqlStatement();

private:
    Connector filterCondition, constraintCondition;

    friend class SelectBuilder<E>;
};

template<typename E>
inline E Select<E>::unique() {
    buildFilterSqlStatement();
    E entity;
    exec([&](const QSqlQuery& query) {
        int resultSize = query.size();
        if (resultSize > 1) {
            printException("unique result size > 1, actual is " + QString::number(resultSize));
        } else {
            query.next();
            E::Tool tool;
            QSqlRecord record = query.record();
            for (int i = 0; i < record.count(); i++) {
                tool.bindValue(entity, record.fieldName(i), record.value(i));
            }
        }
    });
    return entity;
}

template<typename E>
inline E Select<E>::one() {
    buildFilterSqlStatement();
    E entity;
    exec([&](const QSqlQuery& query) {
        if (query.next()) {
            E::Tool tool;
            QSqlRecord record = query.record();
            for (int i = 0; i < record.count(); i++) {
                tool.bindValue(entity, record.fieldName(i), record.value(i));
            }
        }
    });
    return entity;
}

template<typename E>
inline QList<E> Select<E>::list() {
    return QList<E>();
}

template<typename E>
inline void Select<E>::buildFilterSqlStatement() {
    QString sql = "select *from ";
    typename E::Info info;
    sql.append(info.getTableName());

    QVariantList values;

    filterCondition.connect("");
    if (!filterCondition.getConditionStr().isEmpty()) {
        sql.append(" where ").append(filterCondition.getConditionStr());
        values.append(filterCondition.getValues());
    }

    constraintCondition.connect("");
    if (!constraintCondition.getConditionStr().isEmpty()) {
        sql.append(" ").append(constraintCondition.getConditionStr());
        values.append(constraintCondition.getValues());
    }

    setSqlQueryStatement(sql, values);
} 

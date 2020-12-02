#pragma once

#include "BaseQuery.h"

template<typename T>
class SelectBuilder;

template<typename E>
class Select : BaseQuery {
public:
    /// <summary>
    /// 读取一条数据，多条结果将报错，未读到数据将使用默认值
    /// </summary>
    /// <returns></returns>
    E unique();

    /// <summary>
    /// 读取数据并取结果集中一条数据，未读到数据将使用默认值
    /// </summary>
    /// <returns></returns>
    E one();

    /// <summary>
    /// 读取所有符合条件的数据
    /// </summary>
    /// <returns></returns>
    QList<E> list();

    /// <summary>
    /// 原始结果查询，当结果集很大时跳过对象转换，直接读取query结果
    /// </summary>
    /// <param name="callback"></param>
    void raw(std::function<void(QSqlQuery&)> callback);

protected:
    void buildFilterSqlStatement();

protected:
    QString getBindColumns(QVariantList& values);

private:
    Connector columnBind, filterCondition, constraintCondition;

    friend class SelectBuilder<E>;
};

template<typename E>
inline E Select<E>::unique() {
    buildFilterSqlStatement();
    E entity;
    exec([&](QSqlQuery& query) {
        int resultSize = 0;
        while (query.next()) {
            E::Tool tool;
            QSqlRecord record = query.record();
            for (int i = 0; i < record.count(); i++) {
                tool.bindValue(entity, record.fieldName(i), record.value(i));
            }
            resultSize++;
        }
        if (resultSize > 1) {
            printException("unique result size > 1, actual is " + QString::number(resultSize));
        }
    });
    return entity;
}

template<typename E>
inline E Select<E>::one() {
    buildFilterSqlStatement();
    E entity;
    exec([&](QSqlQuery& query) {
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
    buildFilterSqlStatement();
    QList<E> data;
    exec([&](QSqlQuery& query) {
        E::Tool tool;
        while (query.next()) {
            E entity;
            QSqlRecord record = query.record();
            for (int i = 0; i < record.count(); i++) {
                tool.bindValue(entity, record.fieldName(i), record.value(i));
            }
            data << entity;
        }
    });
    return data;
}

template<typename E>
inline void Select<E>::raw(std::function<void(QSqlQuery&)> callback) {
    buildFilterSqlStatement();
    exec(callback);
}

template<typename E>
inline void Select<E>::buildFilterSqlStatement() {
    QString sql = "select %1 from %2";
    typename E::Info info;
    QVariantList values;

    sql = sql.arg(getBindColumns(values));
    sql = sql.arg(info.getTableName());

    filterCondition.connect();
    if (!filterCondition.getConditionStr().isEmpty()) {
        sql.append(" where ").append(filterCondition.getConditionStr());
        values.append(filterCondition.getValues());
    }

    constraintCondition.connect();
    if (!constraintCondition.getConditionStr().isEmpty()) {
        sql.append(" ").append(constraintCondition.getConditionStr());
        values.append(constraintCondition.getValues());
    }

    setSqlQueryStatement(sql, values);
}
template<typename E>
inline QString Select<E>::getBindColumns(QVariantList& values) {
    columnBind.connect();
    if (columnBind.getConditionStr().isEmpty()) {
        return "*";
    }
    values << columnBind.getValues();
    return columnBind.getConditionStr();
}


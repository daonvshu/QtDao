#pragma once

#include "BaseQuery.h"

#include "../macro/macro.h"

template<typename E>
class InsertIntoSelectBuilder;

template<typename E>
class InsertIntoSelect : BaseQuery {
public:
    /// <summary>
    /// 从select结果中复制数据到新表中
    /// </summary>
    /// <returns></returns>
    bool insert();

protected:
    void buildSqlStatement();

    BASE_QUERY_CONSTRUCTOR_DECLARE(InsertIntoSelect);
};

template<typename E>
inline bool InsertIntoSelect<E>::insert() {
    buildSqlStatement();
    bool success = false;
    exec([&](QSqlQuery&) {
        success = true;
    });
    return success;
}

template<typename E>
inline void InsertIntoSelect<E>::buildSqlStatement() {
    typename E::Info info;

    QString sql = "insert into ";
    sql.append(info.getTableName());

    QVariantList values;

    if (!builder->columnBind.isEmpty()) {
        builder->columnBind.connect();
        sql.append(" (").append(builder->columnBind.getConditionStr()).append(")");
        values << builder->columnBind.getValues();
    }
    sql.append(" ").append(builder->fromSelectStatement);
    values << builder->fromSelectValues;

    setSqlQueryStatement(sql, values);
}
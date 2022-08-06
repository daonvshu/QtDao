#pragma once

#include "basequery.h"

#include "../macro/macro.h"

QTDAO_BEGIN_NAMESPACE

template<typename E>
class InsertIntoSelectBuilder;

template<typename E>
class InsertIntoSelect : BaseQuery {
public:
    /// <summary>
    /// select and copy from other select results
    /// </summary>
    /// <returns></returns>
    bool insert();

protected:
    void buildSqlStatement();

    BASE_QUERY_CONSTRUCTOR_DECLARE_W(InsertIntoSelect)
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

QTDAO_END_NAMESPACE
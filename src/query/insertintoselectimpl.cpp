#include "query/insertintoselectimpl.h"

QTDAO_BEGIN_NAMESPACE

void InsertIntoSelectImpl::buildSqlStatement() {

    QString sql = "insert into ";
    sql.append(getTableName());

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
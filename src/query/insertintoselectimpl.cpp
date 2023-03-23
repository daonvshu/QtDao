#include "query/insertintoselectimpl.h"

QTDAO_BEGIN_NAMESPACE

void InsertIntoSelectImpl::buildSqlStatement() {

    QString sql = "insert into ";
    sql.append(getTableName());

    QVariantList values;

    auto& cc = columnConnector();
    if (!cc.isEmpty()) {
        cc.combine();
        sql.append(" (").append(cc.getConditionSegment()).append(")");
        values << cc.getValueList();
    }

    auto& fd = fromBuildData();
    sql.append(" ").append(fd.statement);
    values << fd.values;

    setSqlQueryStatement(sql, values);
}

QTDAO_END_NAMESPACE
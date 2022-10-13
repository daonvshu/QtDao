#include "query/deleteimpl.h"

QTDAO_BEGIN_NAMESPACE

void DeleteImpl::buildDeleteByFilterSqlStatement() {

    QString sql = "delete from ";
    sql.append(getTableName());

    QVariantList value;
    if (!builder->filterCondition.isEmpty()) {
        builder->filterCondition.connect();
        sql.append(" where ").append(builder->filterCondition.getConditionStr());
        value << builder->filterCondition.getValues();
    }
    setSqlQueryStatement(sql, value);
}

void DeleteImpl::buildDeleteEntitiesCondition(const std::function<QVariantList(const QString&)>& fieldColValuesReader) {
    QStringList primaryKeys = getPrimaryKeys();
    Q_ASSERT(!primaryKeys.isEmpty());

    for (const auto& field : primaryKeys) {
        auto fieldValue = fieldColValuesReader(field);
        auto condition =
            EntityCondition(FieldInfo{ field, getTableName() }, "=", fieldValue.size() == 1 ? fieldValue.at(0) : fieldValue);
        builder->filterCondition.append(condition);
    }
}

QTDAO_END_NAMESPACE
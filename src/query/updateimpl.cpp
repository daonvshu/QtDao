#include "query/updateimpl.h"

QTDAO_BEGIN_NAMESPACE

void UpdateImpl::buildUpdateBySetSqlStatement() {
    QString sql = "update ";
    sql.append(getTableName());

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

void UpdateImpl::bindUpdateEntitiesCondition(const std::function<QVariantList(const QString &)> &fieldColValuesReader) {
    Q_ASSERT(builder->setCondition.isEmpty());
    Q_ASSERT(builder->filterCondition.isEmpty());

    QStringList primaryKeys = getPrimaryKeys();
    Q_ASSERT(!primaryKeys.isEmpty());

    QStringList fields = getFields();
    for (const auto& field : fields) {
        auto fieldValue = fieldColValuesReader(field);
        auto condition =
            EntityCondition(FieldInfo{ field, getTableName() }, "=", fieldValue.size() == 1 ? fieldValue.at(0) : fieldValue);
        if (primaryKeys.contains(field)) {
            builder->filterCondition.append(condition);
        } else {
            builder->setCondition.append(condition);
        }
    }
}

QTDAO_END_NAMESPACE

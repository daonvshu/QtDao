#include "query/updateimpl.h"

QTDAO_BEGIN_NAMESPACE

void UpdateImpl::buildUpdateBySetSqlStatement() {
    QString sql = "update ";
    sql.append(getTableName());

    QVariantList value;

    auto& sc = setConnector();
    sc.connect();
    Q_ASSERT(!sc.getConditionStr().isEmpty());
    sql.append(" set ");
    sql.append(sc.getConditionStr());
    value << sc.getValues();

    auto& fc = filterConnector();
    if (!fc.isEmpty()) {
        fc.connect();
        sql.append(" where ").append(fc.getConditionStr());
        value << fc.getValues();
    }
    setSqlQueryStatement(sql, value);
}

void UpdateImpl::bindUpdateEntitiesCondition(const std::function<QVariantList(const QString &)> &fieldColValuesReader) {
    auto& sc = setConnector();
    auto& fc = filterConnector();
    Q_ASSERT(sc.isEmpty());
    Q_ASSERT(fc.isEmpty());

    QStringList primaryKeys = getPrimaryKeys();
    Q_ASSERT(!primaryKeys.isEmpty());

    QStringList fields = getFields();
    for (const auto& field : fields) {
        auto fieldValue = fieldColValuesReader(field);
        auto condition =
            EntityCondition(FieldInfo{ field, getTableName() }, "=", fieldValue.size() == 1 ? fieldValue.at(0) : fieldValue);
        if (primaryKeys.contains(field)) {
            fc.append(condition);
        } else {
            sc.append(condition);
        }
    }
}

QTDAO_END_NAMESPACE

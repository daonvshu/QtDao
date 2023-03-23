#include "query/updateimpl.h"

QTDAO_BEGIN_NAMESPACE

void UpdateImpl::buildUpdateBySetSqlStatement() {
    QString sql = "update ";
    sql.append(getTableName());

    QVariantList value;

    auto& sc = setConnector();
    sc.combine();
    Q_ASSERT(!sc.getConditionSegment().isEmpty());
    sql.append(" set ");
    sql.append(sc.getConditionSegment());
    value << sc.getValueList();

    auto& fc = filterConnector();
    if (!fc.isEmpty()) {
        fc.combine();
        sql.append(" where ").append(fc.getConditionSegment());
        value << fc.getValueList();
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

        auto condition = new OperatorEntityConnector;
        condition->setOperator("=");
        condition->addField(FieldInfo{ field, getTableName() });
        condition->addValue(fieldValue.size() == 1 ? fieldValue.at(0) : fieldValue);

        if (primaryKeys.contains(field)) {
            fc.append(condition->ptr());
        } else {
            sc.append(condition->ptr());
        }
    }
}

QTDAO_END_NAMESPACE

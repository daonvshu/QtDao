#include "query/deleteimpl.h"

QTDAO_BEGIN_NAMESPACE

void DeleteImpl::buildDeleteByFilterSqlStatement() {

    QString sql = "delete from ";
    sql.append(getTableName());

    QVariantList value;
    auto& fc = filterConnector();
    if (!fc.isEmpty()) {
        fc.combine();
        sql.append(" where ").append(fc.getConditionSegment());
        value << fc.getValueList();
    }
    setSqlQueryStatement(sql, value);
}

void DeleteImpl::buildDeleteEntitiesCondition(const std::function<QVariantList(const QString&)>& fieldColValuesReader) {
    QStringList primaryKeys = getPrimaryKeys();
    Q_ASSERT(!primaryKeys.isEmpty());

    auto& fc = filterConnector();
    for (const auto& field : primaryKeys) {
        auto fieldValue = fieldColValuesReader(field);
        auto condition = new OperatorEntityConnector;
        condition->setOperator("=");
        condition->addField(FieldInfo{ field, getTableName() });
        condition->addValue(fieldValue.size() == 1 ? fieldValue.at(0) : fieldValue);
        fc.append(condition->ptr());
    }
}

QTDAO_END_NAMESPACE
#include "EntityCondition.h"

EntityCondition::EntityCondition(
    const QString& fieldName,
    const QString& op,
    const QVariantList& values,
    bool selfOperate
) 
    : fieldName(fieldName)
    , op(op)
    , values(values)
    , selfOperate(selfOperate)
{
}

void EntityCondition::combine(const QString& fieldPrefix) {
    QString str;
    if (selfOperate) {
        str = "%1=%1%2?";
    } else {
        str = "%1%2?";
    }
    combineStr = str.arg(fieldPrefix + fieldName).arg(op);
}

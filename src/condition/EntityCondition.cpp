#include "EntityCondition.h"

EntityCondition::EntityCondition(
    const QString& fieldName,
    const QString& op,
    const QVariantList& values,
    bool selfOperate,
    ConditionType type
) 
    : fieldName(fieldName)
    , op(op)
    , values(values)
    , selfOperate(selfOperate)
    , conditionType(type)
{
}

EntityCondition::EntityCondition(
    const QString& fieldName,
    const QVariantList& values, 
    ConditionType type
)
    :fieldName(fieldName)
    , values(values)
    , conditionType(type)
{
}

void EntityCondition::combine(const QString& fieldPrefix) {
    switch (conditionType) {
    case TypeNormal:
        combineNormal(fieldPrefix);
        break;
    case TypeIn:
        combineIn(fieldPrefix);
        break;
    case TypeBetween:
        combineBetween(fieldPrefix);
        break;
    default:
        break;
    }
}

void EntityCondition::combineNormal(const QString& fieldPrefix) {
    QString str;
    if (selfOperate) {
        str = "%1=%1%2?";
    } else {
        str = "%1%2?";
    }
    combineStr = str.arg(fieldPrefix + fieldName).arg(op);
}

void EntityCondition::combineIn(const QString& fieldPrefix) {
    QString str = "%1 in (%2)";
    combineStr = str
        .arg(fieldPrefix + fieldName)
        .arg(QString("?,").repeated(values.size()).chopped(1));
}

void EntityCondition::combineBetween(const QString& fieldPrefix) {
    QString str = "%1 between ? and ?";
    combineStr = str.arg(fieldPrefix + fieldName);
}
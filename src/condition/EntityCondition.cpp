#include "EntityCondition.h"

EntityCondition::EntityCondition(
    const QString& fieldName,
    const QString& op,
    const QVariantList& values,
    bool selfOperate,
    ConditionType type
) {
    d = new EntityConditionData;
    d->fieldName = fieldName;
    d->op = op;
    d->values = values;
    d->selfOperate = selfOperate;
    d->conditionType = type;
}

EntityCondition::EntityCondition(
    const QString& fieldName,
    const QVariantList& values, 
    ConditionType type
) {
    d = new EntityConditionData;
    d->fieldName = fieldName;
    d->values = values;
    d->conditionType = type;
}

void EntityCondition::combine(const QString& fieldPrefix) {
    switch (d->conditionType) {
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
    if (d->selfOperate) {
        str = "%1=%1%2?";
    } else {
        str = "%1%2?";
    }
    d->combineStr = str.arg(fieldPrefix + d->fieldName).arg(d->op);
}

void EntityCondition::combineIn(const QString& fieldPrefix) {
    QString str = "%1 in (%2)";
    d->combineStr = str
        .arg(fieldPrefix + d->fieldName)
        .arg(QString("?,").repeated(d->values.size()).chopped(1));
}

void EntityCondition::combineBetween(const QString& fieldPrefix) {
    QString str = "%1 between ? and ?";
    d->combineStr = str.arg(fieldPrefix + d->fieldName);
}

QVariantList EntityCondition::getValues() {
    return d->values;
}

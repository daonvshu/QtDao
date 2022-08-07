#include "condition/entitycondition.h"

QTDAO_BEGIN_NAMESPACE

QList<FieldInfo> EntityConditionData::getUsedFields() {
    return fields;
}

EntityCondition::EntityCondition() {
}

EntityCondition::~EntityCondition() {
}

EntityCondition::EntityCondition(
    const FieldInfo& field,
    const QString& op,
    const QVariant& value,
    bool selfOperate
) {
    d = new EntityConditionData;
    d->fields << field;
    d->op = op;
    d->values << value;
    d->selfOperate = selfOperate;
}

EntityCondition::EntityCondition(
    const FieldInfo& field,
    const QVariantList& values, 
    ConditionType type
) {
    d = new EntityConditionData;
    d->fields << field;
    d->values = values;
    d->conditionType = type;
}

EntityCondition::EntityCondition(
    const QVariantList& values,
    ConditionType type
) {
    d = new EntityConditionData;
    d->values = values;
    d->conditionType = type;
}

EntityCondition::EntityCondition(
    const QList<FieldInfo>& fields,
    const QString& op
) {
    d = new EntityConditionData;
    d->fields = fields;
    d->op = op;
}

void EntityCondition::combine() {
    switch (d->conditionType) {
    case TypeNormal:
        combineNormal();
        break;
    case TypeIn:
        combineIn();
        break;
    case TypeBetween:
        combineBetween();
        break;
    case TypeIs:
        combineIs();
        break;
    default:
        break;
    }
}

void EntityCondition::combineNormal() {
    if (d->fields.size() == 1) {
        QString str;
        if (d->selfOperate) {
            str = "%1=%1%2?";
        } else {
            str = "%1%2?";
        }
        d->combineStr = str.arg(d->getField(0)).arg(d->op);
    } else if (d->fields.size() == 2) {
        d->combineStr = QString("%1%2%3")
            .arg(d->getField(0))
            .arg(d->op)
            .arg(d->getField(1));
    }
}

void EntityCondition::combineIn() {
    QString str = "%1 in (%2)";
    d->combineStr = str
        .arg(d->getField(0))
        .arg(QString("?,").repeated(d->values.size()).chopped(1));
}

void EntityCondition::setFieldPrefixGetter(std::function<QString(const QString&)> prefixGetter) {
    d->fieldPrefixGetter = prefixGetter;
}

void EntityCondition::combineBetween() {
    QString str = "%1 between ? and ?";
    d->combineStr = str.arg(d->getField(0));
}

void EntityCondition::combineIs() {
    d->combineStr = QString("%1 is %2")
        .arg(d->getField(0))
        .arg(d->values.at(0).toBool() ? "not null" : "null");
    d->values.clear();
}

QVariantList EntityCondition::getValues() {
    return d->values;
}

QTDAO_END_NAMESPACE
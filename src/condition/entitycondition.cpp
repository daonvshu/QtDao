#include "condition/entitycondition.h"

QTDAO_BEGIN_NAMESPACE

QList<FieldInfo> EntityConnector::getUsedFields() {
    return fields;
}

QVariantList EntityConnector::getValueList() {
    return values;
}

QString EntityConnector::getConditionSegment() {
    return connectedStr;
}

void EntityConnector::addField(const FieldInfo& field) {
    fields << field;
}

void EntityConnector::addValue(const QVariant& value) {
    values << value;
}

QString EntityConnector::getField(int index) const {
    if (fieldPrefixGetter == nullptr) {
        return fields.at(index).name;
    }
    return fieldPrefixGetter(fields.at(index).bindTable) + fields.at(index).name;
}

void OperatorEntityConnector::combine() {
    //same as a>=1
    connectedStr = getField(0) + connectorOp + "?";
}

void SelfOperatorEntityConnector::combine() {
    //same as a=a+1
    connectedStr = getField(0) + "=" + getField(0) + connectorOp + "?";
}

void FieldOperatorEntityConnector::combine() {
    //same as a>=b
    connectedStr = getField(0) + connectorOp + getField(1);
}

void SelfFieldOperatorEntityConnector::combine() {
    //same as a=a+b
    connectedStr = getField(0) + "=" + getField(0) + connectorOp + getField(1);
}

void InEntityConnector::combine() {
    QString str = "%1 in (%2)";
    connectedStr = str.arg(getField(0), QString("?,").repeated(values.size()).chopped(1));
}

void BetweenConnector::combine() {
    connectedStr = getField(0) + " between ? and ?";
}

void IsNullConnector::combine() {
    connectedStr = getField(0) + " is " + (checkIsNull ? "null" : "not null");
}

QTDAO_END_NAMESPACE
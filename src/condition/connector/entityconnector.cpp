#include "condition/connector/entityconnector.h"

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

QTDAO_END_NAMESPACE
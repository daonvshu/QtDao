#include "condition/connector/connectable.h"

QTDAO_BEGIN_NAMESPACE

QList<FieldInfo> Connectable::getUsedFields() {
    return fields;
}

QVariantList Connectable::getValueList() {
    return values;
}

QString Connectable::getConditionSegment() {
    return connectedStr;
}

bool Connectable::isEmpty() {
    return fields.isEmpty();
}

QString Connectable::getField(int index) const {
    if (fieldPrefixGetter == nullptr) {
        return fields.at(index).name;
    }
    return fieldPrefixGetter(fields.at(index).bindTable) + fields.at(index).name;
}

QTDAO_END_NAMESPACE
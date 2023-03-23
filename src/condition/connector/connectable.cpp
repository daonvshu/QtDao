#include "condition/connector/connectable.h"

QTDAO_BEGIN_NAMESPACE

QList<FieldInfo> Connectable::getUsedFields() {
    return d->fields;
}

QVariantList Connectable::getValueList() {
    return d->values;
}

QString Connectable::getConditionSegment() {
    return d->connectedStr;
}

bool Connectable::isEmpty() {
    return d->fields.isEmpty();
}

QString Connectable::getField(int index) const {
    if (d->fieldPrefixGetter == nullptr) {
        return d->fields.at(index).name;
    }
    return d->fieldPrefixGetter(d->fields.at(index).bindTable) + d->fields.at(index).name;
}

QTDAO_END_NAMESPACE
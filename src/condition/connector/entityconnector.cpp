#include "condition/connector/entityconnector.h"

QTDAO_BEGIN_NAMESPACE

void EntityConnector::addField(const FieldInfo& field) {
    d->fields << field;
}

void EntityConnector::addValue(const QVariant& value) {
    d->values << value;
}

QTDAO_END_NAMESPACE
#include "condition/connector/entityconnector.h"

QTDAO_BEGIN_NAMESPACE

void EntityConnector::addField(const FieldInfo& field) {
    fields << field;
}

void EntityConnector::addValue(const QVariant& value) {
    values << value;
}

QTDAO_END_NAMESPACE
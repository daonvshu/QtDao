#include "condition/connector/groupconnector.h"

QTDAO_BEGIN_NAMESPACE

void GroupConnector::combine() {
    auto symbol = connectSymbol();
    for (auto* connector : d->connectors) {
        connector->setFieldPrefixGetter(this->fieldPrefixGetter);
        connector->combine();
        fields << connector->getUsedFields();
        values << connector->getValueList();
        auto segment = connector->getConditionSegment();
        if (dynamic_cast<FilterGroupGroupConnector*>(connector) != nullptr ||
            dynamic_cast<ConstraintGroupGroupConnector*>(connector) != nullptr) {
            connectedStr += '(' + segment + ')';
        } else {
            connectedStr += segment;
        }

        connectedStr += symbol;
    }
    connectedStr = connectedStr.left(connectedStr.length() - symbol.length());
}

void GroupConnector::clear() {
    d->clear();

    fields.clear();
    values.clear();
    connectedStr = "";
}

void HavingGroupConnector::combine() {
    GroupConnector::combine();
    connectedStr.prepend("having ");
}

QTDAO_END_NAMESPACE

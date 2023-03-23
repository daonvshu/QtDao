#include "condition/connector/groupconnector.h"

QTDAO_BEGIN_NAMESPACE

void GroupConnector::combine() {
    d->clear();
    auto symbol = connectSymbol();
    for (auto* connector : p->connectors) {
        connector->setFieldPrefixGetter(d->fieldPrefixGetter);
        connector->combine();
        d->fields << connector->getUsedFields();
        d->values << connector->getValueList();
        auto segment = connector->getConditionSegment();
        if (dynamic_cast<FilterGroupGroupConnector*>(connector) != nullptr ||
            dynamic_cast<ConstraintGroupGroupConnector*>(connector) != nullptr) {
            d->connectedStr += '(' + segment + ')';
        } else {
            d->connectedStr += segment;
        }

        d->connectedStr += symbol;
    }
    d->connectedStr = d->connectedStr.left(d->connectedStr.length() - symbol.length());
}

bool GroupConnector::isEmpty() {
    bool isAllEmpty = Connectable::isEmpty();
    if (!isAllEmpty) {
        return isAllEmpty;
    }

    for (auto* connector : p->connectors) {
        isAllEmpty = connector->isEmpty() && isAllEmpty;
    }

    return isAllEmpty;
}

void HavingGroupConnector::combine() {
    GroupConnector::combine();
    d->connectedStr.prepend("having ");
}

QTDAO_END_NAMESPACE

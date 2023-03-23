#include "condition/connector/groupconnector.h"

QTDAO_BEGIN_NAMESPACE

void GroupConnector::combine() {
    auto symbol = connectSymbol();
    for (auto* connector : connectors) {
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
    for (auto* connector : connectors) {
        auto* ptr = dynamic_cast<GroupConnector*>(connector);
        if (ptr != nullptr) {
            ptr->clear();
        }
    }
    qDeleteAll(connectors);
    connectors.clear();

    fields.clear();
    values.clear();
    connectedStr = "";
}

void GroupConnector::clearByKeepConnectors() {
    connectors.clear();

    fields.clear();
    values.clear();
    connectedStr = "";
}

bool GroupConnector::isEmpty() {
    bool isAllEmpty = Connectable::isEmpty();
    if (!isAllEmpty) {
        return isAllEmpty;
    }

    for (auto* connector : connectors) {
        isAllEmpty = connector->isEmpty() && isAllEmpty;
    }

    return isAllEmpty;
}

void HavingGroupConnector::combine() {
    GroupConnector::combine();
    connectedStr.prepend("having ");
}

QTDAO_END_NAMESPACE

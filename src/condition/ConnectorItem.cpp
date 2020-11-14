#include "ConnectorItem.h"

#include "Connector.h"

ConnectorItemData::ConnectorItemData()
    : connector(nullptr)
    , condition(nullptr) {
}

ConnectorItemData::ConnectorItemData(const ConnectorItemData& other)
    : QSharedData(other)
    , connector(other.connector)
    , condition(other.condition) {
}

ConnectorItemData::~ConnectorItemData() {
    if (connector != nullptr) {
        delete connector;
        connector = nullptr;
    }
    if (condition != nullptr) {
        delete condition;
        condition = nullptr;
    }
}

ConnectorItem::ConnectorItem() {
    d = new ConnectorItemData;
}

ConnectorItem::ConnectorItem(
    Connector* connector, 
    EntityCondition* condition
) {
    d = new ConnectorItemData;
    d->connector = connector;
    d->condition = condition;
}

ConnectorItem::ConnectorItem(const ConnectorItem& other)
    : d(other.d) {
}

ConnectorItem::ConnectorItem(const Connector& connector) {
    d = new ConnectorItemData;
    d->connector = new Connector(connector);
}

ConnectorItem::ConnectorItem(const EntityCondition& condition) {
    d = new ConnectorItemData;
    d->condition = new EntityCondition(condition);
}
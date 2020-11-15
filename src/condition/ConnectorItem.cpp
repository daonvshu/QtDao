#include "ConnectorItem.h"

#include "Connector.h"

ConnectorItemData::ConnectorItemData()
    : connector(nullptr)
    , condition(nullptr) 
    , constraint(nullptr)
{
}

ConnectorItemData::ConnectorItemData(const ConnectorItemData& other)
    : QSharedData(other)
    , connector(other.connector)
    , condition(other.condition) 
    , constraint(other.constraint)
{
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
    if (constraint != nullptr) {
        delete constraint;
        constraint = nullptr;
    }
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

ConnectorItem::ConnectorItem(const ConditionConstraint& constraint) {
    d = new ConnectorItemData;
    d->constraint = new ConditionConstraint(constraint);
}

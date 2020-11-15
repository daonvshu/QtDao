#pragma once

#include <qsharedpointer.h>

class EntityCondition;
class ConditionConstraint;
class Connector;

class ConnectorItemData : public QSharedData {
public:
    ConnectorItemData();
    ConnectorItemData(const ConnectorItemData& other);

    ~ConnectorItemData();

    Connector* connector;
    EntityCondition* condition;
    ConditionConstraint* constraint;
};

class ConnectorItem {
public:
    ConnectorItem(const ConnectorItem& other);

    ConnectorItem(const Connector& connector);
    ConnectorItem(const EntityCondition& condition);
    ConnectorItem(const ConditionConstraint& constraint);

    Connector* getConnector() { return d->connector; }
    EntityCondition* getEntityCondition() { return d->condition; }
    ConditionConstraint* getConstraint() { return d->constraint; }

private:
    QSharedDataPointer<ConnectorItemData> d;
};
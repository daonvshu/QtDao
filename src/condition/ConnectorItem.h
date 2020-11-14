#pragma once

#include <qsharedpointer.h>

class EntityCondition;
class Connector;

class ConnectorItemData : public QSharedData {
public:
    ConnectorItemData();
    ConnectorItemData(const ConnectorItemData& other);

    ~ConnectorItemData();

    Connector* connector;
    EntityCondition* condition;
};

class ConnectorItem {
public:
    ConnectorItem();

    ConnectorItem(
        Connector* connector,
        EntityCondition* condition
    );

    ConnectorItem(const ConnectorItem& other);

    ConnectorItem(
        const Connector& connector
    );

    ConnectorItem(
        const EntityCondition& condition
    );

    Connector* getConnector() { return d->connector; }
    EntityCondition* getEntityCondition() { return d->condition; }

private:
    QSharedDataPointer<ConnectorItemData> d;
};
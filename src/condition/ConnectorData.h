#pragma once

#include <qsharedpointer.h>
#include <qstring.h>
#include <qvariant.h>

enum ConnectorItemType {
    TypeConnector,
    TypeCondition,
    TypeConstraint,
};

class Connector;
class EntityCondition;
class ConditionConstraint;

class ConnectorItem;
class ConnectorData : public QSharedData {
public:
    QList<ConnectorItem> conditions;
    QStringList usedFieldList;
    QString conditionStr;
    QVariantList values;
    QString connectOperator;

    ConnectorData() = default;
    ConnectorData(const ConnectorData& other);
    void operator=(const ConnectorData&) = delete;

    ~ConnectorData();

    void append(const Connector& other);
    void append(const EntityCondition& condition);
    void append(const ConditionConstraint& constraint);
};
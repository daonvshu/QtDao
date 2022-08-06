#pragma once

#include "../global.h"

#include <qsharedpointer.h>
#include <qstring.h>
#include <qvariant.h>

QTDAO_BEGIN_NAMESPACE

enum ConnectorItemType {
    TypeConnector,
    TypeCondition,
    TypeConstraint,
    TypeFunction,
    TypeField,
};

class Connector;
class EntityCondition;
class ConditionConstraint;
class FunctionCondition;
struct FieldInfo;

class ConnectorItem;
class ConnectorData : public QSharedData {
public:
    QList<ConnectorItem> conditions;
    QList<FieldInfo> usedFieldList;
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
    void append(const FunctionCondition& function);
    void append(const FieldInfo& field);
};

QTDAO_END_NAMESPACE
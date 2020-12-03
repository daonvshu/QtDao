#include "ConnectorData.h"

#include "Connector.h"
#include "EntityCondition.h"
#include "ConditionConstraint.h"
#include "FunctionCondition.h"

class ConnectorItem {
public:
    Connector connector;
    EntityCondition condition;
    ConditionConstraint constraint;
    FunctionCondition function;
    FieldInfo field;

    ConnectorItem(const Connector& other);
    ConnectorItem(const EntityCondition& condition);
    ConnectorItem(const ConditionConstraint& constraint);
    ConnectorItem(const FunctionCondition& funtion);
    ConnectorItem(const FieldInfo& field);

    ConnectorItemType type;
};

ConnectorData::ConnectorData(const ConnectorData& other)
    : QSharedData(other)
    , conditions(other.conditions)
    , usedFieldList(other.usedFieldList)
    , conditionStr(other.conditionStr)
    , values(other.values)
    , connectOperator(other.connectOperator)
{
}

ConnectorData::~ConnectorData() {
}

void ConnectorData::append(const Connector& other) {
    conditions.append(other);
}

void ConnectorData::append(const EntityCondition& condition) {
    conditions.append(condition);
}

void ConnectorData::append(const ConditionConstraint& constraint) {
    conditions.append(constraint);
}

void ConnectorData::append(const FunctionCondition& function) {
    conditions.append(function);
}

void ConnectorData::append(const FieldInfo& field) {
    conditions.append(field);
}

ConnectorItem::ConnectorItem(const Connector& other) 
    : connector(other)
    , type(TypeConnector)
{
}

ConnectorItem::ConnectorItem(const EntityCondition& condition)
    : condition(condition)
    , type(TypeCondition)
{
}

ConnectorItem::ConnectorItem(const ConditionConstraint& constraint) 
    : constraint(constraint)
    , type(TypeConstraint)
{
}

ConnectorItem::ConnectorItem(const FunctionCondition& function)
    : function(function)
    , type(TypeFunction)
{
}

ConnectorItem::ConnectorItem(const FieldInfo& field)
    : field(field)
    , type(TypeField)
{
}

Connector::Connector() {
    d = new ConnectorData;
}

Connector::Connector(const char* connectOperator) {
    d = new ConnectorData;
    d->connectOperator = connectOperator;
}

const QList<FieldInfo>& Connector::getUsedFieldNames() {
    return d->usedFieldList;
}

const QString& Connector::getConditionStr() {
    return d->conditionStr;
}

const QVariantList& Connector::getValues() {
    return d->values;
}

bool Connector::isEmpty() {
    return d->conditions.isEmpty();
}

void Connector::appendConnector() {
    if (d->connectOperator.isEmpty()) {
        d->conditionStr.append(' ');
        return;
    }
    if (d->connectOperator == ',') {
        d->conditionStr.append(d->connectOperator);
        return;
    }
    d->conditionStr.append(' ').append(d->connectOperator).append(' ');
};


void Connector::connect(std::function<QString(const QString&)> prefixGetter) {
    QMutableListIterator<ConnectorItem> it(d->conditions);
    while (it.hasNext()) {
        if (it.hasPrevious()) {
            appendConnector();
        }
        auto item = it.next();
        switch (item.type) {
        case TypeConnector:
            {
                auto connector = &item.connector;
                connector->connect(prefixGetter);
                d->usedFieldList.append(connector->d->usedFieldList);
                if (d->conditionStr.isEmpty()) {
                    d->conditionStr.append(connector->d->conditionStr);
                } else {
                    d->conditionStr.append('(').append(connector->d->conditionStr).append(')');
                }
                d->values.append(connector->d->values);
            }
            break;
        case TypeCondition:
            {
                auto condition = &item.condition;
                condition->setFieldPrefixGetter(prefixGetter);
                condition->combine();
                d->usedFieldList.append(condition->d->getUsedFields());
                d->conditionStr.append(condition->d->combineStr);
                d->values.append(condition->getValues());
            }
            break;
        case TypeConstraint:
            {
                auto constraint = &item.constraint;
                constraint->setFieldPrefixGetter(prefixGetter);
                constraint->combine();
                //d->usedFieldList.append(constraint->d->getUsedFields());    //not use
                d->conditionStr.append(constraint->d->combineStr);
                d->values.append(constraint->getValues());
            }
            break;
        case TypeFunction:
            {
                auto function = &item.function;
                function->setFieldPrefixGetter(prefixGetter);
                function->combine();
                d->usedFieldList.append(function->d->getUsedFields());
                d->conditionStr.append(function->d->combineStr);
                d->values.append(function->d->values);
            }
            break;
        case TypeField:
            {
                auto field = item.field;
                QString fieldName;
                if (prefixGetter != nullptr) {
                    fieldName = prefixGetter(field.bindTable) + field.name;
                } else {
                    fieldName = field.name;
                }
                d->conditionStr.append(fieldName);
                d->usedFieldList.append(field);
            }
            break;
        }

    }
}

void Connector::clear() {
    d->conditions.clear();
}

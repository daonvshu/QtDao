#pragma once

#include "EntityCondition.h"
#include "ConnectorItem.h"

class ConnectorData : public QSharedData {
public:
    ConnectorData() {
    }

    ConnectorData(const ConnectorData& other) 
        : QSharedData(other)
        , conditions(other.conditions)
        , conditionStr(other.conditionStr)
        , values(other.values)
        , connectOperator(other.connectOperator)
    {
    }

    ~ConnectorData() {
    }

    QList<ConnectorItem> conditions;
    QString conditionStr;
    QVariantList values;
    QString connectOperator;
};

class Connector {
public:
    Connector(const char* connectOperator) {
        d = new ConnectorData;
        d->connectOperator = connectOperator;
    }

    Connector(const Connector& other)
        : d (other.d) {
    }

    ~Connector() {
    }

    template<typename C, typename... T>
    Connector& append(const C& condition, const T&... conditions) {
        Q_STATIC_ASSERT((
            std::is_same<C, EntityCondition>::value ||
            std::is_same<C, Connector>::value
        ));
        appendCondition(condition);
        return append(conditions...);
    }

    Connector& append() {
        return *this;
    }

    void connect(const QString& fieldPrefix) {
        QMutableListIterator<ConnectorItem> it(d->conditions);
        while (it.hasNext()) {
            if (it.hasPrevious()) {
                appendConnector();
            }
            auto& item = it.next();
            auto condition = item.getEntityCondition();
            if (condition != nullptr) {
                condition->combine(fieldPrefix);
                d->conditionStr.append(condition->combineStr);
                d->values.append(condition->values);
                continue;
            }

            auto connector = item.getConnector();
            if (connector != nullptr) {
                connector->connect(fieldPrefix);
                d->conditionStr.append(connector->d->conditionStr);
                d->values.append(connector->d->values);
                continue;
            }

        }
        d->conditionStr.prepend('(');
        d->conditionStr.append(')');
    }

    const QString& getConditionStr() {
        return d->conditionStr;
    }

    const QVariantList& getValues() {
        return d->values;
    }

protected:
    virtual void appendConnector() {
        d->conditionStr.append(' ').append(d->connectOperator).append(' ');
    };

private:
    template<typename T>
    void appendCondition(const T& condition);

private:
    QSharedDataPointer<ConnectorData> d;
};

template<>
inline void Connector::appendCondition(const EntityCondition& condition) {
    d->conditions.append(condition);
}

template<>
inline void Connector::appendCondition(const Connector& connector) {
    d->conditions.append(connector);
}


#define _and(...) Connector("and").append(__VA_ARGS__)
#define _or(...) Connector("or").append(__VA_ARGS__)


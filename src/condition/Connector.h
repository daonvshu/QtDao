#pragma once

#include "ConnectorData.h"
#include "EntityField.h"

template<typename T>
class SelectBuilder;

class FunctionCondition;

class Connector {
public:
    Connector();
    Connector(const char* connectOperator);

    ~Connector() {
    }

    template<typename C, typename... T>
    Connector& append(const C& condition, const T&... conditions) {
        Q_STATIC_ASSERT((
            std::is_same<C, EntityCondition>::value ||
            std::is_same<C, ConditionConstraint>::value ||
            std::is_same<C, Connector>::value ||
            std::is_same<C, FunctionCondition>::value
        ));
        d->append(condition);
        return append(conditions...);
    }

    Connector& append() {
        return *this;
    }

    void connect(const QString& fieldPrefix);

    const QStringList& getUsedFieldNames();
    const QString& getConditionStr();
    const QVariantList& getValues();

    bool isEmpty();

protected:
    virtual void appendConnector();

private:
    template<typename E, typename... Args>
    Connector& appendCol(const EntityField<E>& field, const Args&... condition) {
        d->append(field());
        return appendCol(condition...);
    }

    template<typename T, typename... Args>
    Connector& appendCol(const T& function, const Args&... condition) {
        Q_STATIC_ASSERT((std::is_same<T, FunctionCondition>::value));
        d->append(function);
        return appendCol(condition...);
    }

    Connector& appendCol() {
        return *this;
    }

    template<typename T>
    friend class SelectBuilder;

private:
    QSharedDataPointer<ConnectorData> d;
};

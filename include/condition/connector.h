#pragma once

#include "../global.h"

#include "connectordata.h"
#include "entityfield.h"
#include <functional>

QTDAO_BEGIN_NAMESPACE

class BaseQueryBuilder;
class FunctionCondition;

class Connector {
public:
    Connector();
    Connector(const char* connectOperator);
    Connector(const Connector& other);

    ~Connector();

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

    void connect(std::function<QString(const QString&)> prefixGetter = nullptr);
    void clear();

    const QList<FieldInfo>& getUsedFieldNames();
    const QString& getConditionStr();
    const QVariantList& getValues();

    bool isEmpty();

protected:
    virtual void appendConnector();

public:
    template<typename E, typename... Args>
    Connector& appendCol(const EntityField<E>& field, const Args&... condition) {
        d->append(FieldInfo{ field.name, field.bindTable });
        return appendCol(condition...);
    }

    template<typename T, typename... Args>
    Connector& appendCol(const T& function, const Args&... condition) {
        Q_STATIC_ASSERT((std::is_same<T, FunctionCondition>::value));
        d->append(function);
        return appendCol(condition...);
    }

    template<typename... Args>
    Connector& appendCol(const FieldInfo& info, const Args&... args) {
        d->append(info);
        return appendCol(args...);
    }

    Connector& appendCol() {
        return *this;
    }

    template<typename E> friend class UpsertBuilder;

private:
    QSharedDataPointer<ConnectorData> d;
};

QTDAO_END_NAMESPACE
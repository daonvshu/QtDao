#pragma once

#include "ConnectorData.h"

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
            std::is_same<C, Connector>::value
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
    QSharedDataPointer<ConnectorData> d;
};

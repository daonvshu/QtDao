#pragma once

#include "connectable.h"
#include "fieldconnector.h"

#include "entityconnector.h"
#include "constraintconnector.h"
#include "../functioncondition.h"

#include <qshareddata.h>

QTDAO_BEGIN_NAMESPACE

struct GroupConnectorData : QSharedData {
public:
    GroupConnectorData() = default;

    GroupConnectorData(const GroupConnectorData& other): QSharedData(other) {
        connectors << other.connectors;
    }

    ~GroupConnectorData() {
        qDeleteAll(connectors);
    }

    void clear() {
        qDeleteAll(connectors);
        connectors.clear();
    }

    QList<Connectable*> connectors;
};

class GroupConnector : public Connectable {
public:
    GroupConnector() {
        d = new GroupConnectorData;
    }

    void combine() override;

    void clear();

protected:
    virtual QString connectSymbol() = 0;

protected:
    QSharedDataPointer<GroupConnectorData> d;
};

class FieldGroupConnector : public GroupConnector {
public:
    template<typename T, typename... Args>
    void appends(T&& t, Args&&... args) {
        append(std::forward<T>(t));
        appends(args...);
    }

    void appends() {}

    template<typename E>
    void append(const EntityField<E>& field) {
        auto connector = new FieldConnector;
        connector->setField(field);
        d->connectors << connector;
    }

    void append(const FieldInfo& fieldInfo) {
        auto connector = new FieldConnector;
        connector->setField(fieldInfo);
        d->connectors << connector;
    }

    void append(const FunctionConnector& connector) {
        d->connectors << (new FunctionConnector(connector))->ptr();
    }


protected:
    QString connectSymbol() override {
        return ",";
    }
};

class SetGroupConnector : public GroupConnector {
public:
    template<typename T, typename... Args>
    void appends(T&& t, Args&&... args) {
        append(std::forward<T>(t));
        appends(args...);
    }

    void appends() {}

    void append(EntityConnector* connector) {
        d->connectors << connector;
    }

    void append(const FunctionConnector& connector) {
        d->connectors << (new FunctionConnector(connector))->ptr();
    }

protected:
    QString connectSymbol() override {
        return ",";
    }
};

class FilterGroupGroupConnector;
class FilterGroupConnector : public GroupConnector {
public:
    template<typename T, typename... Args>
    void appends(T&& t, Args&&... args) {
        append(std::forward<T>(t));
        appends(args...);
    }

    void appends() {}

    void append(EntityConnector* connector) {
        d->connectors << connector;
    }

    void append(const FunctionConnector& connector) {
        d->connectors << (new FunctionConnector(connector))->ptr();
    }

    void append(FilterGroupGroupConnector* connector);

protected:
    QString connectSymbol() override {
        return " and ";
    }
};

class FilterGroupGroupConnector : public FilterGroupConnector {
};

inline void FilterGroupConnector::append(FilterGroupGroupConnector* connector) {
    d->connectors << connector;
}

class AndFilterGroupConnector : public FilterGroupGroupConnector {
};

class OrFilterGroupConnector : public FilterGroupGroupConnector {
protected:
    QString connectSymbol() override {
        return " or ";
    }
};

class ConstraintGroupGroupConnector;
class HavingGroupConnector;
class ConstraintGroupConnector : public GroupConnector {
public:
    template<typename T, typename... Args>
    void appends(T&& t, Args&&... args) {
        append(std::forward<T>(t));
        appends(args...);
    }

    void appends() {}

    void append(ConstraintConnector* connector) {
        d->connectors << connector->ptr();
    }

    void append(ConstraintGroupGroupConnector* connector);

    void append(HavingGroupConnector* connector);

protected:
    QString connectSymbol() override {
        return ",";
    }
};

class ConstraintGroupGroupConnector : public ConstraintGroupConnector {
};

inline void ConstraintGroupConnector::append(ConstraintGroupGroupConnector* connector) {
    d->connectors << connector;
}

class HavingGroupConnector : public FilterGroupConnector {
public:
    void combine() override;

protected:
    QString connectSymbol() override {
        return "";
    }
};

inline void ConstraintGroupConnector::append(HavingGroupConnector* connector) {
    d->connectors << connector;
}

QTDAO_END_NAMESPACE
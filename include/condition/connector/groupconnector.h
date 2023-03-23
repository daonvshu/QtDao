#pragma once

#include "connectable.h"
#include "fieldconnector.h"

#include "entityconnector.h"
#include "constraintconnector.h"
#include "../functioncondition.h"

QTDAO_BEGIN_NAMESPACE

struct GroupConnectorData : QSharedData {

    GroupConnectorData() = default;

    ~GroupConnectorData() {
        qDeleteAll(connectors);
    }

    QList<Connectable*> connectors;
};

class GroupConnector : public Connectable {
public:
    GroupConnector() {
        p = new GroupConnectorData;
    }

    void combine() override;

    bool isEmpty() override;

protected:
    virtual QString connectSymbol() = 0;

protected:
    QExplicitlySharedDataPointer<GroupConnectorData> p;
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
        p->connectors << connector;
    }

    void append(const FieldInfo& fieldInfo) {
        auto connector = new FieldConnector;
        connector->setField(fieldInfo);
        p->connectors << connector;
    }

    void append(const FunctionConnector& connector) {
        p->connectors << (new FunctionConnector(connector))->ptr();
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
        p->connectors << connector;
    }

    void append(const FunctionConnector& connector) {
        p->connectors << (new FunctionConnector(connector))->ptr();
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
        p->connectors << connector;
    }

    void append(const FunctionConnector& connector) {
        p->connectors << (new FunctionConnector(connector))->ptr();
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
    p->connectors << connector;
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
        p->connectors << connector->ptr();
    }

    void append(ConstraintGroupGroupConnector* connector);

    void append(HavingGroupConnector* connector);

protected:
    QString connectSymbol() override {
        return " ";
    }
};

class ConstraintGroupGroupConnector : public ConstraintGroupConnector {
};

inline void ConstraintGroupConnector::append(ConstraintGroupGroupConnector* connector) {
    p->connectors << connector;
}

class HavingGroupConnector : public FilterGroupConnector {
public:
    void combine() override;
};

inline void ConstraintGroupConnector::append(HavingGroupConnector* connector) {
    p->connectors << connector;
}

QTDAO_END_NAMESPACE
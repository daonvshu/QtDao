#pragma once

#include "connectable.h"
#include "fieldconnector.h"

#include "entityconnector.h"
#include "constraintconnector.h"
#include "../functioncondition.h"

QTDAO_BEGIN_NAMESPACE

class GroupConnector : public Connectable {
public:
    void combine() override;

    void clear();

    void clearByKeepConnectors();

    bool isEmpty() override;

protected:
    virtual QString connectSymbol() = 0;

protected:
    QList<Connectable*> connectors;
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
        connectors << connector;
    }

    void append(const FieldInfo& fieldInfo) {
        auto connector = new FieldConnector;
        connector->setField(fieldInfo);
        connectors << connector;
    }

    void append(const FunctionConnector& connector) {
        connectors << (new FunctionConnector(connector))->ptr();
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
        connectors << connector;
    }

    void append(const FunctionConnector& connector) {
        connectors << (new FunctionConnector(connector))->ptr();
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
        connectors << connector;
    }

    void append(const FunctionConnector& connector) {
        connectors << (new FunctionConnector(connector))->ptr();
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
    connectors << connector;
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
        connectors << connector->ptr();
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
    connectors << connector;
}

class HavingGroupConnector : public FilterGroupConnector {
public:
    void combine() override;
};

inline void ConstraintGroupConnector::append(HavingGroupConnector* connector) {
    connectors << connector;
}

QTDAO_END_NAMESPACE
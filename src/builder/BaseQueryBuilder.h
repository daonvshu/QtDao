#pragma once

#include "../condition/EntityCondition.h"
#include "../condition/Connector.h"
#include "../condition/FunctionCondition.h"
#include "../condition/EntityField.h"

#include "../query/Select.h"

template<typename T> class Insert;
template<typename T> class Select;
template<typename T> class Update;
template<typename T> class Delete;
template<typename... E> class Join;

class BaseQueryBuilder {
public:
    BaseQueryBuilder() 
        : setThrowable(false)
        , setCondition(",")
        , columnBind(",")
        , filterCondition("and")
        , onCondition("and")
    {
    }

    ~BaseQueryBuilder() {}

protected:
    template<typename... Args>
    void set(const EntityCondition& condition, const Args&... args);

    template<typename... Args>
    void filter(const EntityCondition& condition, const Args&... args);

    template<typename... Args>
    void filter(const Connector& condition, const Args&... args);

    template<typename... Args>
    void filter(const FunctionCondition& condition, const Args&... args);

    template<typename... Args>
    void on(const EntityCondition& condition, const Args&... args);

    template<typename... Args>
    void on(const Connector& condition, const Args&... args);

    template<typename... Args>
    void on(const FunctionCondition& condition, const Args&... args);

    template<typename... Args>
    void with(const ConditionConstraint& constaint, const Args&... args);

    template<typename Col, typename... Args>
    void column(const Col& function, const Args&... args);

    virtual void set() {}
    virtual void filter() {}
    virtual void with() {}
    virtual void column() {}
    virtual void on() {}

    template<typename T>
    void from(Select<T>& select);

protected:
    bool setThrowable;
    Connector setCondition, columnBind, filterCondition, constraintCondition, onCondition;
    QString fromSelectStatement;
    QVariantList fromSelectValues;
    QString fromSelectAs;

    template<typename T> friend class Insert;
    template<typename T> friend class Select;
    template<typename T> friend class Update;
    template<typename T> friend class Delete;
    template<typename... E> friend class Join;
};

template<typename ...Args>
inline void BaseQueryBuilder::set(const EntityCondition& condition, const Args & ...args) {
    setCondition.append(condition);
    return set(args...);
}

template<typename ...Args>
inline void BaseQueryBuilder::filter(const EntityCondition& condition, const Args & ...args) {
    filterCondition.append(condition);
    filter(args...);
}

template<typename ...Args>
inline void BaseQueryBuilder::filter(const Connector& condition, const Args & ...args) {
    filterCondition.append(condition);
    filter(args...);
}

template<typename ...Args>
inline void BaseQueryBuilder::filter(const FunctionCondition& condition, const Args & ...args) {
    filterCondition.append(condition);
    filter(args...);
}

template<typename ...Args>
inline void BaseQueryBuilder::on(const EntityCondition& condition, const Args & ...args) {
    onCondition.append(condition);
    on(args...);
}

template<typename ...Args>
inline void BaseQueryBuilder::on(const Connector& condition, const Args & ...args) {
    onCondition.append(condition);
    on(args...);
}

template<typename ...Args>
inline void BaseQueryBuilder::on(const FunctionCondition& condition, const Args & ...args) {
    onCondition.append(condition);
    on(args...);
}

template<typename ...Args>
inline void BaseQueryBuilder::with(const ConditionConstraint& constaint, const Args & ...args) {
    constraintCondition.append(constaint);
    with(args...);
}

template<typename Col, typename ...Args>
inline void BaseQueryBuilder::column(const Col& function, const Args & ...args) {
    columnBind.appendCol(function);
    column(args...);
}

template<typename T>
inline void BaseQueryBuilder::from(Select<T>& select) {
    select.buildFilterSqlStatement();
    fromSelectStatement = select.statement;
    fromSelectValues = select.values;
    if (select.builder->fromSelectAs.isEmpty()) {
        fromSelectAs = "sel_" + T::Info::getTableName();
    } else {
        fromSelectAs = "sel_" + select.builder->fromSelectAs;
    }
}

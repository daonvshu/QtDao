#pragma once

#include "../condition/EntityCondition.h"
#include "../condition/Connector.h"
#include "../condition/FunctionCondition.h"
#include "../condition/EntityField.h"

#include "../query/BaseQuery.h"

#define QUERY_BUILDER_USE(T, name) \
template<typename... Args>\
T& name(const Args&... args) {\
    __super::name(args...);\
    return *this;\
}

#define QUERY_BUILDER_USE_THROWABLE(T)\
T& throwable() {\
    setThrowable = true;\
    return *this;\
}

#define QUERY_BUILDER_USE_SET(T)          QUERY_BUILDER_USE(T, set)
#define QUERY_BUILDER_USE_FILTER(T)     QUERY_BUILDER_USE(T, filter)
#define QUERY_BUILDER_USE_WITH(T)       QUERY_BUILDER_USE(T, with)
#define QUERY_BUILDER_USE_COLUMN(T)   QUERY_BUILDER_USE(T, column)
#define QUERY_BUILDER_USE_ON(T)            QUERY_BUILDER_USE(T, on)

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

protected:
    bool setThrowable;
    Connector setCondition, columnBind, filterCondition, constraintCondition, onCondition;
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

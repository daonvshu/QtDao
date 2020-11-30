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

#define QUERY_BUILDER_USE_SET(T)          QUERY_BUILDER_USE(T, set)
#define QUERY_BUILDER_USE_FILTER(T)     QUERY_BUILDER_USE(T, filter)
#define QUERY_BUILDER_USE_WITH(T)       QUERY_BUILDER_USE(T, with)
#define QUERY_BUILDER_USE_COLUMN(T)   QUERY_BUILDER_USE(T, column)

template<typename T, template<typename> class Query>
class BaseQueryBuilder {
public:
    BaseQueryBuilder() 
        : setThrowable(false)
        , setCondition(",")
        , columnBind(",")
        , filterCondition("and")
    {
    }

    BaseQueryBuilder& throwable();

    virtual Query<T> build() = 0;

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
    void with(const ConditionConstraint& constaint, const Args&... args);

    template<typename Col, typename... Args>
    void column(const Col& function, const Args&... args);

    void set();

    void filter();

    void with();

    void column();

protected:
    bool setThrowable;
    Connector setCondition, columnBind, filterCondition, constraintCondition;
};

template<typename T, template<typename> class Query>
inline BaseQueryBuilder<T, Query>& BaseQueryBuilder<T, Query>::throwable() {
    setThrowable = true;
    return *this;
}

template<typename T, template<typename> class Query>
template<typename ...Args>
inline void BaseQueryBuilder<T, Query>::set(const EntityCondition& condition, const Args & ...args) {
    setCondition.append(condition);
    return set(args...);
}

template<typename T, template<typename> class Query>
template<typename ...Args>
inline void BaseQueryBuilder<T, Query>::filter(const EntityCondition& condition, const Args & ...args) {
    filterCondition.append(condition);
    filter(args...);
}

template<typename T, template<typename> class Query>
template<typename ...Args>
inline void BaseQueryBuilder<T, Query>::filter(const Connector& condition, const Args & ...args) {
    filterCondition.append(condition);
    filter(args...);
}

template<typename T, template<typename> class Query>
template<typename ...Args>
inline void BaseQueryBuilder<T, Query>::filter(const FunctionCondition& condition, const Args & ...args) {
    filterCondition.append(condition);
    filter(args...);
}

template<typename T, template<typename> class Query>
template<typename ...Args>
inline void BaseQueryBuilder<T, Query>::with(const ConditionConstraint& constaint, const Args & ...args) {
    constraintCondition.append(constaint);
    with(args...);
}

template<typename T, template<typename> class Query>
template<typename Col, typename ...Args>
inline void BaseQueryBuilder<T, Query>::column(const Col& function, const Args & ...args) {
    columnBind.appendCol(function);
    column(args...);
}

template<typename T, template<typename> class Query>
inline void BaseQueryBuilder<T, Query>::set() {
}

template<typename T, template<typename> class Query>
inline void BaseQueryBuilder<T, Query>::filter() {
}

template<typename T, template<typename> class Query>
inline void BaseQueryBuilder<T, Query>::with() {
}

template<typename T, template<typename> class Query>
inline void BaseQueryBuilder<T, Query>::column() {
}
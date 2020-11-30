#pragma once

#include "QueryBuilder.h"

#include "../query/Update.h"

template<typename T>
class UpdateBuilder : public QueryBuilder<T, Update> {
public:
    UpdateBuilder() : setCondition(","), filterCondition("and") {
    }

    template<typename... Args>
    UpdateBuilder& set(const EntityCondition& condition, const Args&... args);

    UpdateBuilder& set();

    template<typename... Args>
    UpdateBuilder& filter(const EntityCondition& condition, const Args&... args);

    template<typename... Args>
    UpdateBuilder& filter(const Connector& condition, const Args&... args);

    template<typename... Args>
    UpdateBuilder& filter(const FunctionCondition& condition, const Args&... args);

    UpdateBuilder& filter();

    Update<T> build() override;

private:
    Connector setCondition, filterCondition;
};

template<typename T>
template<typename ...Args>
inline UpdateBuilder<T>& UpdateBuilder<T>::set(const EntityCondition& condition, const Args & ...args) {
    setCondition.append(condition);
    return set(args...);
}

template<typename T>
inline UpdateBuilder<T>& UpdateBuilder<T>::set() {
    return *this;
}

template<typename T>
template<typename ...Args>
inline UpdateBuilder<T>& UpdateBuilder<T>::filter(const EntityCondition& condition, const Args & ...args) {
    filterCondition.append(condition);
    return filter(args...);
}

template<typename T>
template<typename ...Args>
inline UpdateBuilder<T>& UpdateBuilder<T>::filter(const Connector& condition, const Args & ...args) {
    filterCondition.append(condition);
    return filter(args...);
}

template<typename T>
template<typename ...Args>
inline UpdateBuilder<T>& UpdateBuilder<T>::filter(const FunctionCondition& condition, const Args & ...args) {
    filterCondition.append(condition);
    return filter(args...);
}

template<typename T>
inline UpdateBuilder<T>& UpdateBuilder<T>::filter() {
    return *this;
}

template<typename T>
inline Update<T> UpdateBuilder<T>::build() {
    Update<T> query;
    query.setCondition = setCondition;
    query.filterCondition = filterCondition;
    query.queryThrowable = setThrowable;
    return query;
}

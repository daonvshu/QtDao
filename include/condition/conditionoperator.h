#pragma once

#include "../global.h"

#include "connector/groupconnector.h"
#include "conditionconstraint.h"

template<typename... T>
inline dao::FilterGroupGroupConnector* _and(T&&... t) {
    auto ptr = new dao::AndFilterGroupConnector;
    ptr->appends(std::forward<T>(t)...);
    return ptr;
}

template<typename... T>
inline dao::FilterGroupGroupConnector* _or(T&&... t) {
    auto ptr = new dao::OrFilterGroupConnector;
    ptr->appends(std::forward<T>(t)...);
    return ptr;
}

template<typename... T>
inline dao::ConstraintGroupGroupConnector* _constraint(T&&... t) {
    auto ptr = new dao::ConstraintGroupGroupConnector;
    ptr->appends(std::forward<T>(t)...);
    return ptr;
}

inline dao::ConstraintConnector* _limit(int a, int b) {
    return new dao::LimitConstraintConnector(a, b);
}

inline dao::ConstraintConnector* _limit(int a) {
    return new dao::LimitConstraintConnector(a);
}

template<typename... E>
inline dao::ConstraintConnector* _orderBy(const dao::EntityField<E>&... n) {
    auto ptr = new dao::OrderByConstraintConnector;
    ptr->orderBy(n...);
    return ptr;
}

template<typename... N>
inline dao::ConstraintConnector* _groupBy(const dao::EntityField<N>&... n) {
    auto ptr = new dao::GroupByConstraintConnector;
    ptr->groupBy(n...);
    return ptr;
}

template<typename... T>
inline dao::HavingGroupConnector* _having(T&&... t) {
    auto ptr = new dao::HavingGroupConnector;
    ptr->appends(std::forward<T>(t)...);
    return ptr;
}

inline dao::FunctionConnector _fun(const QString& expressions) {
    return dao::FunctionConnector(expressions);
}
#pragma once

#include "../global.h"

#include "connector.h"
#include "entityfield.h"
#include "entitycondition.h"
#include "conditionconstraint.h"
#include "functioncondition.h"

template<typename... T>
inline dao::Connector _and(const T&... t) {
    return dao::Connector("and").append(t...);
}

template<typename... T>
inline dao::Connector _or(const T&... t) {
    return dao::Connector("or").append(t...);
}

template<typename... T>
inline dao::Connector _constraint(const T&... t) {
    return dao::Connector("").append(t...);
}

inline dao::ConditionConstraint _limit(int a, int b) {
    return dao::ConditionConstraint::limit(a, b);
}

inline dao::ConditionConstraint _limit(int a) {
    return dao::ConditionConstraint::limit(a);
}

template<typename... E>
inline dao::ConditionConstraint _orderBy(const dao::EntityField<E>&... n) {
    return dao::ConditionConstraint::orderBy(n...);
}

template<typename... N>
inline dao::ConditionConstraint _groupBy(const dao::EntityField<N>&... n) {
    return dao::ConditionConstraint::groupBy(n...);
}

inline dao::ConditionConstraint _having(const dao::EntityCondition& condition) {
    return dao::ConditionConstraint::having(condition);
}

inline dao::ConditionConstraint _having(const dao::FunctionCondition& condition) {
    return dao::ConditionConstraint::having(condition);
}

inline dao::FunctionCondition _fun(const QString& expressions) {
    return { expressions };
}
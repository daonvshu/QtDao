#include "Connector.h"
#include "EntityField.h"
#include "EntityCondition.h"
#include "ConditionConstraint.h"
#include "FunctionCondition.h"

template<typename... T>
inline Connector _and(const T&... t) {
    return Connector("and").append(t...);
}

template<typename... T>
inline Connector _or(const T&... t) {
    return Connector("or").append(t...);
}

template<typename... T>
inline Connector _constraint(const T&... t) {
    return Connector("").append(t...);
}

inline ConditionConstraint _limit(int a, int b) {
    return ConditionConstraint::limit(a, b);
}

inline ConditionConstraint _limit(int a) {
    return ConditionConstraint::limit(a);
}

template<typename... E>
inline ConditionConstraint _orderBy(const EntityField<E>&... n) {
    return ConditionConstraint::orderBy(n...);
}

template<typename... N>
inline ConditionConstraint _groupBy(const EntityField<N>&... n) {
    return ConditionConstraint::groupBy(n...);
}

inline FunctionCondition _fun(const QString& expressions) {
    return FunctionCondition(expressions);
}
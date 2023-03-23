#pragma once

#include "filterbuilder.h"

QTDAO_BEGIN_NAMESPACE

//same as FilterBuilder
template<typename T>
class OnConditionBuilder : FilterBuilderImpl<OnConditionBuilder<T>> {
public:
    /**
     * add some on filter conditions, example:
     * fields.name == "Alice",  _and(fields.name == "Alice", fields.age == 1),
     * _fun("%1 + %2 > 100").field(fields.score1, fields.score2)
     * @tparam Args condition type, EntityCondition/Connector/FunctionCondition
     * @param args conditions
     * @return this
     */
    template<typename...Args>
    T& on(Args&& ...args) {
        this->doFilter(std::forward<Args>(args)...);
        return static_cast<T&>(*this);
    }

    /**
     * add a on filter condition, using enabled to add optional
     * @tparam Arg condition type, EntityCondition/Connector/FunctionCondition
     * @param enabled add condition if enabled
     * @param arg conditions
     * @return this
     */
    template<typename Arg>
    T& on(bool enabled, Arg&& arg) {
        this->doFilter(enabled, std::forward<Arg>(arg));
        return static_cast<T&>(*this);
    }

    template<template<typename> class, typename>
    friend class BuilderReaderProvider;

    template<template<typename...> class, typename...>
    friend class BuilderJbReaderProvider;

    template<typename... E>
    friend class JoinBuilder;
};

QTDAO_END_NAMESPACE
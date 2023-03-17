#pragma once

#include "../../global.h"

#include "../../condition/entitycondition.h"
#include "../../condition/connector.h"
#include "../../condition/functioncondition.h"

QTDAO_BEGIN_NAMESPACE

template<typename T>
class FilterBuilderImpl {
protected:
    /**
     * use fields to add filter conditions, example: fields.name == "Alice"
     * @tparam Args condition type, EntityCondition/Connector/FunctionCondition
     * @param condition field condition
     * @param args other types of conditions
     */
    template<typename... Args>
    void doFilter(const EntityCondition& condition, const Args&... args) {
        filterCondition.append(condition);
        doFilter(args...);
    }

    /**
     * use fields to add filter conditions, using enabled to add optional
     * @param enabled add condition if enabled
     * @param condition field condition
     */
    void doFilter(bool enabled, const EntityCondition& condition) {
        if (enabled) {
            filterCondition.append(condition);
        }
    }

    /**
     * use combination condition to add filter conditions,
     * example: _and(fields.name == "Alice", fields.age == 1)
     * @tparam Args condition type, EntityCondition/Connector/FunctionCondition
     * @param condition combination condition
     * @param args other types of conditions
     */
    template<typename... Args>
    void doFilter(const Connector& condition, const Args&... args) {
        filterCondition.append(condition);
        doFilter(args...);
    }

    /**
     * use combination condition to add filter conditions, using enabled to add optional
     * @param enabled add condition if enabled
     * @param condition combination condition
     */
    void doFilter(bool enabled, const Connector& condition) {
        if (enabled) {
            filterCondition.append(condition);
        }
    }

    /**
     * use custom condition to add filter conditions,
     * example: _fun("%1 + %2 > 100").field(fields.score1, fields.score2)
     * @tparam Args condition type, EntityCondition/Connector/FunctionCondition
     * @param condition function condition
     * @param args other types of conditions
     */
    template<typename... Args>
    void doFilter(const FunctionCondition& condition, const Args&... args) {
        filterCondition.append(condition);
        doFilter(args...);
    }

    /**
     * use custom condition to add filter conditions, using enabled to add optional
     * @param enabled add condition if enabled
     * @param condition function condition
     */
    void doFilter(bool enabled, const FunctionCondition& condition) {
        if (enabled) {
            filterCondition.append(condition);
        }
    }

    /**
     * end function recursion
     */
    virtual void doFilter() {}

protected:
    //use 'and' connect conditions
    Connector filterCondition{"and"};
};

template<typename T>
class FilterBuilder : FilterBuilderImpl<FilterBuilder<T>> {
public:
    /**
     * add some filter conditions, example:
     * fields.name == "Alice",  _and(fields.name == "Alice", fields.age == 1),
     * _fun("%1 + %2 > 100").field(fields.score1, fields.score2)
     * @tparam Args condition type, EntityCondition/Connector/FunctionCondition
     * @param args conditions
     * @return this
     */
    template<typename...Args>
    T& filter(const Args &...args) {
        this->doFilter(args...);
        return static_cast<T&>(*this);
    }

    /**
     * add a filter condition, using enabled to add optional
     * @tparam Arg condition type, EntityCondition/Connector/FunctionCondition
     * @param enabled add condition if enabled
     * @param arg conditions
     * @return this
     */
    template<typename Arg>
    T& filter(bool enabled, const Arg &arg) {
        this->doFilter(enabled, arg);
        return static_cast<T&>(*this);
    }

protected:
    template<template<typename> class, typename>
    friend class BuilderReaderProvider;

    template<template<typename...> class, typename...>
    friend class BuilderJbReaderProvider;

    template<typename... E>
    friend class JoinBuilder;

    template<typename E>
    friend class CountBuilder;
};

QTDAO_END_NAMESPACE
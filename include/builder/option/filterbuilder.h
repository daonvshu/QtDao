#pragma once

#include "../../global.h"

#include "../../condition/entitycondition.h"
#include "../../condition/functioncondition.h"

QTDAO_BEGIN_NAMESPACE

template<typename T>
class FilterBuilderImpl {
protected:
    /**
     * use fields to add filter conditions, example: fields.name == "Alice"
     * @tparam F condition type, EntityConnector/FunctionConnector/FilterGroupGroupConnector
     * @tparam Args types
     * @param condition field/combination/function condition
     * @param args other types of conditions
     */
    template<typename F, typename... Args>
    void doFilter(F&& condition, Args&&... args) {
        filterCondition.append(std::forward<F>(condition));
        doFilter(args...);
    }

    /**
     * use fields to add filter conditions, using enabled to add optional
     * @tparam F condition type, EntityConnector/FunctionConnector/FilterGroupGroupConnector
     * @param enabled add condition if enabled
     * @param condition field/combination/function condition
     */
    template<typename F>
    void doFilter(bool enabled, F&& condition) {
        if (enabled) {
            filterCondition.append(std::forward<F>(condition));
        }
    }

    /**
     * end function recursion
     */
    virtual void doFilter() {}

protected:
    //use 'and' connect conditions
    FilterGroupConnector filterCondition;
};

template<typename T>
class FilterBuilder : FilterBuilderImpl<FilterBuilder<T>> {
public:
    /**
     * add some filter conditions, example:
     * fields.name == "Alice",  _and(fields.name == "Alice", fields.age == 1),
     * _fun("%1 + %2 > 100").field(fields.score1, fields.score2)
     * @tparam Args condition type, EntityConnector/FunctionConnector/FilterGroupGroupConnector
     * @param args conditions
     * @return this
     */
    template<typename...Args>
    T& filter(Args&& ...args) {
        this->doFilter(std::forward<Args>(args)...);
        return static_cast<T&>(*this);
    }

    /**
     * add a filter condition, using enabled to add optional
     * @tparam Arg condition type, EntityConnector/FunctionConnector/FilterGroupGroupConnector
     * @param enabled add condition if enabled
     * @param arg conditions
     * @return this
     */
    template<typename Arg>
    T& filter(bool enabled, Arg&& arg) {
        this->doFilter(enabled, std::forward<Arg>(arg));
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
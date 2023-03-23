#pragma once

#include "../../global.h"

#include "../../condition/connector/groupconnector.h"

QTDAO_BEGIN_NAMESPACE

template<typename T>
class SetBuilder {
public:
    /**
     * use fields to add set conditions, example:
     * fields.name = "Alice", fields.age = 18
     * @tparam E type of EntityConnector or FunctionConnector
     * @tparam Args types
     * @param condition set conditions
     * @param args other fields of conditions
     */
    template<typename E, typename... Args>
    T& set(E&& condition, Args&&... args){
        setCondition.append(std::forward<E>(condition));
        return set(args...);
    }

    /**
     * add a set conditions, using enabled to add optional
     * @tparam E type of EntityConnector or FunctionConnector
     * @param enabled add condition if enabled
     * @param condition set conditions
     */
    template<typename E>
    T& set(bool enabled, E&& condition){
        if (enabled) {
            setCondition.append(std::forward<E>(condition));
        }
        return static_cast<T&>(*this);
    }

    /**
     * end function recursion
     */
    T& set() {
        return static_cast<T&>(*this);
    }

private:
    //use ',' connect conditions
    SetGroupConnector setCondition;

    template<template<typename> class, typename>
    friend class BuilderReaderProvider;

    template<template<typename...> class, typename...>
    friend class BuilderJbReaderProvider;
};

QTDAO_END_NAMESPACE
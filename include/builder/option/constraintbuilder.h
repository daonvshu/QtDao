#pragma once

#include "../../global.h"

#include "../../condition/connector/groupconnector.h"

QTDAO_BEGIN_NAMESPACE

template<typename T>
class ConstraintBuilder {
public:
    /**
     * add constraint conditions, example:
     * _limit(5, 20), _orderBy(field.name, field.age)
     * @tparam Cst type of ConstraintConnector or ConstraintGroupGroupConnector or HavingGroupConnector
     * @tparam Args types
     * @param constraint constraint condition
     * @param args other types of constraint condition
     * @return this
     */
    template<typename Cst, typename... Args>
    T& with(Cst&& constraint, Args&&... args) {
        constraintCondition.append(std::forward<Cst>(constraint));
        return with(args...);
    }

    /**
     * add constraint condition, using enabled to add condition optional
     * @tparam Cst type of ConstraintConnector or ConstraintGroupGroupConnector or HavingGroupConnector
     * @param enabled add condition if enabled
     * @param constraint condition
     * @return this
     */
    template<typename Cst>
    T& with(bool enabled, Cst&& constraint) {
        if (enabled) {
            constraintCondition.append(std::forward<Cst>(constraint));
        }
        return static_cast<T&>(*this);
    }

    /**
     * end function recursion
     * @return this
     */
    T& with() {
        return static_cast<T&>(*this);
    }

private:
    //use default connect conditions
    ConstraintGroupConnector constraintCondition;

    template<template<typename> class, typename>
    friend class BuilderReaderProvider;

    template<template<typename...> class, typename...>
    friend class BuilderJbReaderProvider;
};

QTDAO_END_NAMESPACE
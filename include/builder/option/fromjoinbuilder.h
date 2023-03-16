#pragma once

#include "frombuilder.h"

QTDAO_BEGIN_NAMESPACE

template<bool SelfEntity, template<typename> class T, typename... E>
class FromJoinBuilder;

template<template<typename> class T, typename... E>
class FromJoinBuilder<true, T, E...> : FromBuilder {
public:
    T<E...>& from(Join<E...> &join) {
        from(static_cast<JoinImpl&>(join));
        return static_cast<T<E...>&>(*this);
    }
};

template<template<typename> class T, typename... E>
class FromJoinBuilder<false, T, E...> : FromBuilder {
public:
    template<typename... E2>
    T<E...>& from(Join<E2...> &join) {
        from(static_cast<JoinImpl&>(join));
        return static_cast<T<E...>&>(*this);
    }
};

QTDAO_END_NAMESPACE
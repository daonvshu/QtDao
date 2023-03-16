#pragma once

#include "frombuilder.h"

QTDAO_BEGIN_NAMESPACE

template<bool SelfEntity, template<typename> class T, typename E>
class FromSelectBuilder;

template<template<typename> class T, typename E>
class FromSelectBuilder<true, T, E> : FromBuilder {
public:
    T<E>& from(Select<E> &select) {
        from(static_cast<SelectImpl&>(select));
        return static_cast<T<E>&>(*this);
    }

    T<E>& from(Select<E> &&select) {
        return from(select);
    }
};

template<template<typename> class T, typename E>
class FromSelectBuilder<false, T, E> : FromBuilder {
public:
    template<typename E2>
    T<E>& from(Select<E2> &select) {
        from(static_cast<SelectImpl&>(select));
        return static_cast<T<E>&>(*this);
    }

    template<typename E2>
    T<E>& from(Select<E2> &&select) {
        return from(select);
    }
};

QTDAO_END_NAMESPACE
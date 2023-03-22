#pragma once

#include "frombuilder.h"

QTDAO_BEGIN_NAMESPACE

template<template<typename> class T, typename E>
class FromSelfSelectBuilder : protected FromBuilder {
public:
    T<E>& from(Select<E> &select) {
        fromSelect(static_cast<SelectImpl&>(select));
        return static_cast<T<E>&>(*this);
    }

    T<E>& from(Select<E> &&select) {
        return from(select);
    }

    template<typename... E2>
    T<E>& from(Join<E2...> &join) {
        fromJoin(static_cast<JoinImpl&>(join));
        return static_cast<T<E>&>(*this);
    }

    T<E>& from(RecursiveQueryBuilder& builder) {
        fromBuilder(builder);
        return static_cast<T<E>&>(*this);
    }

    T<E>& from(RecursiveQueryBuilder&& builder) {
        fromBuilder(builder);
        return static_cast<T<E>&>(*this);
    }

    template<template<typename> class, typename>
    friend class BuilderReaderProvider;

    template<template<typename...> class, typename...>
    friend class BuilderJbReaderProvider;
};

template<typename T>
class FromE2SelectBuilder : protected FromBuilder {
public:
    template<typename E>
    T& from(Select<E> &select) {
        fromSelect(static_cast<SelectImpl&>(select));
        return static_cast<T&>(*this);
    }

    template<typename E>
    T& from(Select<E> &&select) {
        return from(select);
    }

    template<typename... E>
    T& from(Join<E...> &join) {
        fromJoin(static_cast<JoinImpl&>(join));
        return static_cast<T&>(*this);
    }

    T& from(RecursiveQueryBuilder& builder) {
        fromBuilder(builder);
        return static_cast<T&>(*this);
    }

    T& from(RecursiveQueryBuilder&& builder) {
        fromBuilder(builder);
        return static_cast<T&>(*this);
    }

    template<template<typename> class, typename>
    friend class BuilderReaderProvider;

    template<template<typename...> class, typename...>
    friend class BuilderJbReaderProvider;
};

template<template<typename...> class T, typename... E>
class FromEsSelectBuilder : protected FromBuilder {
public:
    template<typename E2>
    T<E...>& from(Select<E2> &select) {
        fromSelect(static_cast<SelectImpl&>(select));
        return static_cast<T<E...>&>(*this);
    }

    template<typename E2>
    T<E...>& from(Select<E2> &&select) {
        return from(select);
    }

    T<E...>& from(RecursiveQueryBuilder& builder) {
        fromBuilder(builder);
        return static_cast<T<E...>&>(*this);
    }

    T<E...>& from(RecursiveQueryBuilder&& builder) {
        return from(builder);
    }

    template<template<typename> class, typename>
    friend class BuilderReaderProvider;

    template<template<typename...> class, typename...>
    friend class BuilderJbReaderProvider;
};

QTDAO_END_NAMESPACE
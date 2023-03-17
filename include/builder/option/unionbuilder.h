#pragma once

#include "../../global.h"

#include <qvariant.h>

QTDAO_BEGIN_NAMESPACE

struct UnionBuildData {
    QString statement;
    QVariantList values;
    bool unionAll = false;
};

template<typename E>
class Select;

template<typename... E>
class Join;

class SelectImpl;
class JoinImpl;

class UnionBuilderImpl {
protected:
    void unionWithSelect(SelectImpl& select, bool unionAll);

    void unionWithJoin(JoinImpl& join, bool unionAll);

private:
    UnionBuildData unionData;

    template<template<typename> class, typename>
    friend class BuilderReaderProvider;

    template<template<typename...> class, typename...>
    friend class BuilderJbReaderProvider;
};

template<typename T>
class UnionBuilder : UnionBuilderImpl {
public:
    template<typename E2>
    T& unionSelect(Select<E2>& select, bool unionAll = false) {
        unionWithSelect(select, unionAll);
        return static_cast<T&>(*this);
    }

    template<typename E2>
    T& unionSelect(Select<E2>&& select, bool unionAll = false) {
        return unionSelect(select, unionAll);
    }

    template<typename...E2>
    T& unionSelect(Join<E2...>& join, bool unionAll = false) {
        unionWithJoin(join, unionAll);
        return static_cast<T&>(*this);
    }

    template<typename...E2>
    T& unionSelect(Join<E2...>&& join, bool unionAll = false) {
        return unionSelect(join, unionAll);
    }

    template<template<typename> class, typename>
    friend class BuilderReaderProvider;

    template<template<typename...> class, typename...>
    friend class BuilderJbReaderProvider;
};

QTDAO_END_NAMESPACE
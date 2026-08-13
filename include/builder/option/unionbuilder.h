#pragma once

#include "../../global.h"

#include <qvariant.h>

QTDAO_BEGIN_NAMESPACE

struct UnionBuildData {
    QString statement;
    QVariantList values;
    bool unionAll = false;

    void clear() {
        statement = QString();
        values.clear();
        unionAll = false;
    }
};

template<typename E>
class Select;

template<typename... E>
class Join;

class SelectImpl;
class JoinImpl;

class UnionBuilderImpl {
protected:
    void unionWithSelect(SelectImpl& select, bool unionAll, qint64 sessionId);

    void unionWithJoin(JoinImpl& join, bool unionAll, qint64 sessionId);

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
        unionWithSelect(select, unionAll, static_cast<T&>(*this).querySessionId);
        return static_cast<T&>(*this);
    }

    template<typename E2>
    T& unionSelect(Select<E2>&& select, bool unionAll = false) {
        return unionSelect(select, unionAll);
    }

    template<typename...E2>
    T& unionSelect(Join<E2...>& join, bool unionAll = false) {
        unionWithJoin(join, unionAll, static_cast<T&>(*this).querySessionId);
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

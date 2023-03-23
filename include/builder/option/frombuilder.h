#pragma once

#include "../../global.h"

#include <qvariant.h>

QTDAO_BEGIN_NAMESPACE

struct FromBuildData {
    QString statement;
    QVariantList values;
    QString asName;
    bool recursiveQuery = false;

    void clear() {
        statement = QString();
        values.clear();
        asName = QString();
        recursiveQuery = false;
    }
};

class SelectImpl;
class JoinImpl;
class RecursiveQueryBuilder;

class FromBuilder {
public:
    void fromDataClear() {
        fromData.clear();
    }

protected:
    virtual void fromSelect(SelectImpl& select);

    virtual void fromJoin(JoinImpl& join);

    virtual void fromBuilder(RecursiveQueryBuilder& builder);

protected:
    FromBuildData fromData;

    template<template<typename> class, typename>
    friend class BuilderReaderProvider;

    template<template<typename...> class, typename...>
    friend class BuilderJbReaderProvider;

    template<typename E>
    friend class CountBuilder;
};

QTDAO_END_NAMESPACE
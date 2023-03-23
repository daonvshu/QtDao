#pragma once

#include "global.h"

#include "../../builder/option/columnbuilder.h"
#include "../../builder/option/constraintbuilder.h"
#include "../../builder/option/filterbuilder.h"
#include "../../builder/option/onconditionbuilder.h"
#include "../../builder/option/setbuilder.h"

#include "../../builder/option/frombuilder.h"
#include "../../builder/option/unionbuilder.h"

QTDAO_BEGIN_NAMESPACE

#pragma warning(push)
#pragma warning(disable:4312)
class BuilderReaderInterface {
protected:
    virtual FieldGroupConnector& columnConnector() {
        return *reinterpret_cast<FieldGroupConnector*>(c);
    }

    virtual FieldGroupConnector& conflictColumnConnector() {
        return *reinterpret_cast<FieldGroupConnector*>(c);
    }

    virtual FieldGroupConnector& updateColumnConnector() {
        return *reinterpret_cast<FieldGroupConnector*>(c);
    }

    virtual SetGroupConnector& setConnector() {
        return *reinterpret_cast<SetGroupConnector*>(c);
    }

    virtual FilterGroupConnector& filterConnector() {
        return *reinterpret_cast<FilterGroupConnector*>(c);
    }

    virtual ConstraintGroupConnector& constraintConnector() {
        return *reinterpret_cast<ConstraintGroupConnector*>(c);
    }

    virtual FromBuildData& fromBuildData() {
        return *reinterpret_cast<FromBuildData*>(c);
    }

    virtual UnionBuildData& unionBuildData() {
        return *reinterpret_cast<UnionBuildData*>(c);
    }

private:
    int c = 0;
};
#pragma warning(pop)

template<template<typename> class B, typename E>
class BuilderReaderProvider;

template<template<typename...> class B, typename... E>
class BuilderJbReaderProvider;

template<typename E>
class DeleteBuilder;

template<typename E>
class BuilderReaderProvider<DeleteBuilder, E> : protected virtual BuilderReaderInterface {
public:
    explicit BuilderReaderProvider(DeleteBuilder<E>* builder): builder(builder) {}

    FilterGroupConnector& filterConnector() override {
        return builder->filterCondition;
    }

protected:
    DeleteBuilder<E>* builder;
};

template<typename E>
class InsertBuilder;

template<typename E>
class BuilderReaderProvider<InsertBuilder, E> : protected virtual BuilderReaderInterface {
public:
    explicit BuilderReaderProvider(InsertBuilder<E>* builder): builder(builder) {}

    SetGroupConnector& setConnector() override {
        return builder->setCondition;
    }

protected:
    InsertBuilder<E>* builder;
};

template<typename E>
class InsertIntoSelectBuilder;

template<typename E>
class BuilderReaderProvider<InsertIntoSelectBuilder, E> : protected virtual BuilderReaderInterface {
public:
    explicit BuilderReaderProvider(InsertIntoSelectBuilder<E>* builder): builder(builder) {}

    FieldGroupConnector& columnConnector() override {
        return builder->columnCondition;
    }

    FromBuildData& fromBuildData() override {
        return builder->fromData;
    }

protected:
    InsertIntoSelectBuilder<E>* builder;
};

template<typename... E>
class JoinBuilder;

template<typename... E>
class BuilderJbReaderProvider<JoinBuilder, E...>: protected virtual BuilderReaderInterface {
public:
    explicit BuilderJbReaderProvider(JoinBuilder<E...>* builder): builder(builder) {}

    FieldGroupConnector& columnConnector() override {
        return builder->columnCondition;
    }

    FilterGroupConnector& filterConnector() override {
        return builder->template FilterBuilder<JoinBuilder<E...>>::filterCondition;
    }

    ConstraintGroupConnector& constraintConnector() override {
        return builder->constraintCondition;
    }

    FromBuildData& fromBuildData() override {
        return builder->template FromEsSelectBuilder<JoinBuilder, E...>::fromData;
    }

    UnionBuildData& unionBuildData() override {
        return builder->unionData;
    }

protected:
    JoinBuilder<E...>* builder;
};

template<typename E>
class SelectBuilder;

template<typename E>
class BuilderReaderProvider<SelectBuilder, E> : protected virtual BuilderReaderInterface {
public:
    explicit BuilderReaderProvider(SelectBuilder<E>* builder): builder(builder) {}

    FieldGroupConnector& columnConnector() override {
        return builder->columnCondition;
    }

    FilterGroupConnector& filterConnector() override {
        return builder->filterCondition;
    }

    ConstraintGroupConnector& constraintConnector() override {
        return builder->constraintCondition;
    }

    FromBuildData& fromBuildData() override {
        return builder->fromData;
    }

    UnionBuildData& unionBuildData() override {
        return builder->unionData;
    }

protected:
    SelectBuilder<E>* builder;
};

template<typename E>
class UpdateBuilder;

template<typename E>
class BuilderReaderProvider<UpdateBuilder, E> : protected virtual BuilderReaderInterface {
public:
    explicit BuilderReaderProvider(UpdateBuilder<E>* builder): builder(builder) {}

    SetGroupConnector& setConnector() override {
        return builder->setCondition;
    }

    FilterGroupConnector& filterConnector() override {
        return builder->filterCondition;
    }

protected:
    UpdateBuilder<E>* builder;
};

template<typename E>
class UpsertBuilder;

template<typename E>
class BuilderReaderProvider<UpsertBuilder, E> : protected virtual BuilderReaderInterface {
public:
    explicit BuilderReaderProvider(UpsertBuilder<E>* builder): builder(builder) {}

    FieldGroupConnector& conflictColumnConnector() override {
        return builder->template ConflictColumnBuilder<UpsertBuilder<E>>::columnCondition;
    }

    FieldGroupConnector& updateColumnConnector() override {
        return builder->template UpdateColumnBuilder<UpsertBuilder<E>>::columnCondition;
    }

    SetGroupConnector& setConnector() override {
        return builder->setCondition;
    }

    FilterGroupConnector& filterConnector() override {
        return builder->filterCondition;
    }

protected:
    UpsertBuilder<E>* builder;
};

QTDAO_END_NAMESPACE
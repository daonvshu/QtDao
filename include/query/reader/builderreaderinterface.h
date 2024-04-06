#pragma once

#include "global.h"

#include "../../builder/option/columnbuilder.h"
#include "../../builder/option/constraintbuilder.h"
#include "../../builder/option/filterbuilder.h"
#include "../../builder/option/onconditionbuilder.h"
#include "../../builder/option/setbuilder.h"

#include "../../builder/option/frombuilder.h"
#include "../../builder/option/unionbuilder.h"

#include "../../builder/option/sessionbuilder.h"

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

    virtual qint64 getSessionId() {
        qFatal("not implement!");
        return -1;
    }

private:
    int c = 0;
};
#pragma warning(pop)

template<template<typename> class B, typename E>
class BuilderReaderProvider;

template<template<typename...> class B, typename... E>
class BuilderJbReaderProvider;

template<typename I, typename B>
class BuilderInstanceHolder : protected virtual I {
public:
    explicit BuilderInstanceHolder(B&& builder): builder(builder) {}
    virtual ~BuilderInstanceHolder() = default;

protected:
    B builder;
};

template<typename E>
class DeleteBuilder;

template<typename E>
class BuilderReaderProvider<DeleteBuilder, E> : protected BuilderInstanceHolder<BuilderReaderInterface, DeleteBuilder<E>> {
public:
    using BuilderInstanceHolder<BuilderReaderInterface, DeleteBuilder<E>>::BuilderInstanceHolder;

    FilterGroupConnector& filterConnector() override {
        return this->builder.filterCondition;
    }

    qint64 getSessionId() override {
        return this->builder.querySessionId;
    }
};

template<typename E>
class InsertBuilder;

template<typename E>
class BuilderReaderProvider<InsertBuilder, E> : protected BuilderInstanceHolder<BuilderReaderInterface, InsertBuilder<E>> {
public:
    using BuilderInstanceHolder<BuilderReaderInterface, InsertBuilder<E>>::BuilderInstanceHolder;

    SetGroupConnector& setConnector() override {
        return this->builder.setCondition;
    }

    qint64 getSessionId() override {
        return this->builder.querySessionId;
    }
};

template<typename E>
class InsertIntoSelectBuilder;

template<typename E>
class BuilderReaderProvider<InsertIntoSelectBuilder, E> : protected BuilderInstanceHolder<BuilderReaderInterface, InsertIntoSelectBuilder<E>> {
public:
    using BuilderInstanceHolder<BuilderReaderInterface, InsertIntoSelectBuilder<E>>::BuilderInstanceHolder;

    FieldGroupConnector& columnConnector() override {
        return this->builder.columnCondition;
    }

    FromBuildData& fromBuildData() override {
        return this->builder.fromData;
    }

    qint64 getSessionId() override {
        return this->builder.querySessionId;
    }
};

template<typename... E>
class JoinBuilder;

template<typename... E>
class BuilderJbReaderProvider<JoinBuilder, E...>: protected BuilderInstanceHolder<BuilderReaderInterface, JoinBuilder<E...>> {
public:
    using BuilderInstanceHolder<BuilderReaderInterface, JoinBuilder<E...>>::BuilderInstanceHolder;

    FieldGroupConnector& columnConnector() override {
        return this->builder.columnCondition;
    }

    FilterGroupConnector& filterConnector() override {
        return this->builder.FilterBuilder<JoinBuilder<E...>>::filterCondition;
    }

    ConstraintGroupConnector& constraintConnector() override {
        return this->builder.constraintCondition;
    }

    FromBuildData& fromBuildData() override {
        return this->builder.FromEsSelectBuilder<JoinBuilder, E...>::fromData;
    }

    UnionBuildData& unionBuildData() override {
        return this->builder.unionData;
    }

    qint64 getSessionId() override {
        return this->builder.querySessionId;
    }
};

template<typename E>
class SelectBuilder;

template<typename E>
class BuilderReaderProvider<SelectBuilder, E> : protected BuilderInstanceHolder<BuilderReaderInterface, SelectBuilder<E>> {
public:
    using BuilderInstanceHolder<BuilderReaderInterface, SelectBuilder<E>>::BuilderInstanceHolder;

    FieldGroupConnector& columnConnector() override {
        return this->builder.columnCondition;
    }

    FilterGroupConnector& filterConnector() override {
        return this->builder.filterCondition;
    }

    ConstraintGroupConnector& constraintConnector() override {
        return this->builder.constraintCondition;
    }

    FromBuildData& fromBuildData() override {
        return this->builder.fromData;
    }

    UnionBuildData& unionBuildData() override {
        return this->builder.unionData;
    }

    qint64 getSessionId() override {
        return this->builder.querySessionId;
    }
};

template<typename E>
class UpdateBuilder;

template<typename E>
class BuilderReaderProvider<UpdateBuilder, E> : protected BuilderInstanceHolder<BuilderReaderInterface, UpdateBuilder<E>> {
public:
    using BuilderInstanceHolder<BuilderReaderInterface, UpdateBuilder<E>>::BuilderInstanceHolder;

    SetGroupConnector& setConnector() override {
        return this->builder.setCondition;
    }

    FilterGroupConnector& filterConnector() override {
        return this->builder.filterCondition;
    }

    qint64 getSessionId() override {
        return this->builder.querySessionId;
    }
};

template<typename E>
class UpsertBuilder;

template<typename E>
class BuilderReaderProvider<UpsertBuilder, E> : protected BuilderInstanceHolder<BuilderReaderInterface, UpsertBuilder<E>> {
public:
    using BuilderInstanceHolder<BuilderReaderInterface, UpsertBuilder<E>>::BuilderInstanceHolder;

    FieldGroupConnector& conflictColumnConnector() override {
        return this->builder.ConflictColumnBuilder<UpsertBuilder<E>>::columnCondition;
    }

    FieldGroupConnector& updateColumnConnector() override {
        return this->builder.UpdateColumnBuilder<UpsertBuilder<E>>::columnCondition;
    }

    SetGroupConnector& setConnector() override {
        return this->builder.setCondition;
    }

    FilterGroupConnector& filterConnector() override {
        return this->builder.filterCondition;
    }

    qint64 getSessionId() override {
        return this->builder.querySessionId;
    }
};

QTDAO_END_NAMESPACE
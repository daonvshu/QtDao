#pragma once

#include "../../global.h"

#include "../../condition/connector.h"

QTDAO_BEGIN_NAMESPACE

template<typename T>
class ColumnBuilderImpl {
protected:
    /**
     * use fields or custom function to add select columns, example:
     * fields.name, _fun("sum(%1) as scores").field(fields.score)
     * @tparam Col type of fields or custom function
     * @tparam Args condition type, Entity/FunctionCondition
     * @param function fields or custom function
     * @param args other types of columns
     */
    template<typename Col, typename... Args>
    void doColumn(const Col& function, const Args&... args){
        columnCondition.appendCol(function);
        doColumn(args...);
    }

    /**
     * use fields or custom function to add select columns, using enabled to add column optional
     * @tparam Col type of fields or custom function
     * @param enabled add column if enabled
     * @param function fields or custom function
     */
    template<typename Col>
    void doColumn(bool enabled, const Col& function){
        if (enabled) {
            columnCondition.appendCol(function);
        }
    }

    /**
     * end function recursion
     */
    void doColumn() {}

protected:
    //use ',' connect conditions
    Connector columnCondition{","};
};

template<typename T>
class ColumnBuilder : ColumnBuilderImpl<ColumnBuilder<T>> {
public:
    /**
     * use fields or custom function to add select columns, example:
     * fields.name, _fun("sum(%1) as scores").field(fields.score)
     * @tparam Args condition type, Entity/FunctionCondition
     * @param args fields or custom function
     * @return this
     */
    template<typename...Args>
    T& column(const Args &...args) {
        this->doColumn(args...);
        return static_cast<T&>(*this);
    }

    /**
     * use fields or custom function to add select columns, using enabled to add column optional
     * @tparam Arg type of fields or custom function
     * @param enabled add column if enabled
     * @param arg fields or custom function
     * @return this
     */
    template<typename Arg>
    T& column(bool enabled, const Arg &arg) {
        this->doColumn(enabled, arg);
        return static_cast<T&>(*this);
    }

    /**
     * add target entity all columns
     * @tparam E entity type
     */
    template<typename E>
    T& columnAll(){
        QString tbName = E::Info::getTableName();
        QStringList fields = E::Info::getFields();
        for (const auto& field : fields) {
            this->columnCondition.appendCol(FieldInfo{ field, tbName });
        }
        return static_cast<T&>(*this);
    }

protected:
    template<template<typename> class, typename>
    friend class BuilderReaderProvider;

    template<template<typename...> class, typename...>
    friend class BuilderJbReaderProvider;
};

template<typename T>
class ConflictColumnBuilder : ColumnBuilderImpl<ConflictColumnBuilder<T>> {
public:
    /**
     * use fields or custom function to add upsert conflict columns, example:
     * fields.name, fields.age
     * @tparam Args condition type, Entity
     * @param args fields
     * @return this
     */
    template<typename...Args>
    T& conflictColumns(const Args &...args) {
        this->doColumn(args...);
        return static_cast<T&>(*this);
    }

    /**
     * use fields or custom function to add upsert conflict columns, using enabled to add column optional
     * @tparam Arg type of Entity
     * @param enabled add column if enabled
     * @param arg fields
     * @return this
     */
    template<typename Arg>
    T& conflictColumns(bool enabled, const Arg &arg) {
        this->doColumn(enabled, arg);
        return static_cast<T&>(*this);
    }

    template<template<typename> class, typename>
    friend class BuilderReaderProvider;

    template<template<typename...> class, typename...>
    friend class BuilderJbReaderProvider;
};

template<typename T>
class UpdateColumnBuilder : ColumnBuilderImpl<UpdateColumnBuilder<T>> {
public:
    /**
     * use fields or custom function to add upsert update columns, example:
     * fields.name, fields.age
     * @tparam Args condition type, Entity
     * @param args fields
     * @return this
     */
    template<typename...Args>
    T& updateColumns(const Args &...args) {
        this->doColumn(args...);
        return static_cast<T&>(*this);
    }

    /**
     * use fields or custom function to add upsert update columns, using enabled to add column optional
     * @tparam Arg type of Entity
     * @param enabled add column if enabled
     * @param arg fields
     * @return this
     */
    template<typename Arg>
    T& updateColumns(bool enabled, const Arg &arg) {
        this->doColumn(enabled, arg);
        return static_cast<T&>(*this);
    }

    template<template<typename> class, typename>
    friend class BuilderReaderProvider;

    template<template<typename...> class, typename...>
    friend class BuilderJbReaderProvider;
};

QTDAO_END_NAMESPACE
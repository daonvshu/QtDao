#pragma once

#include "../../global.h"

#include "../../condition/connector/groupconnector.h"

QTDAO_BEGIN_NAMESPACE

template<typename T>
class ColumnBuilderImpl {
protected:
    /**
     * use fields or custom function to add select columns, example:
     * fields.name, _fun("sum(%1) as scores").field(fields.score)
     * @tparam Col type of EntityField<E> or FunctionConnector
     * @tparam Args condition types
     * @param col fields or custom function
     * @param args other types of columns
     */
    template<typename Col, typename... Args>
    void doColumn(Col&& col, Args&&... args){
        columnCondition.append(std::forward<Col>(col));
        doColumn(args...);
    }

    /**
     * use fields or custom function to add select columns, using enabled to add column optional
     * @tparam Col type of EntityField<E> or FunctionConnector
     * @param enabled add column if enabled
     * @param col fields or custom function
     */
    template<typename Col>
    void doColumn(bool enabled, Col&& col){
        if (enabled) {
            columnCondition.append(std::forward<Col>(col));
        }
    }

    /**
     * end function recursion
     */
    void doColumn() {}

protected:
    //use ',' connect conditions
    FieldGroupConnector columnCondition;
};

template<typename T>
class ColumnBuilder : ColumnBuilderImpl<ColumnBuilder<T>> {
public:
    /**
     * use fields or custom function to add select columns, example:
     * fields.name, _fun("sum(%1) as scores").field(fields.score)
     * @tparam Args condition type, EntityField<E> or FunctionConnector
     * @param args fields or custom function
     * @return this
     */
    template<typename...Args>
    T& column(Args&&... args) {
        this->doColumn(std::forward<Args>(args)...);
        return static_cast<T&>(*this);
    }

    /**
     * use fields or custom function to add select columns, using enabled to add column optional
     * @tparam Arg condition type, EntityField<E> or FunctionConnector
     * @param enabled add column if enabled
     * @param arg fields or custom function
     * @return this
     */
    template<typename Arg>
    T& column(bool enabled, Arg&& arg) {
        this->doColumn(enabled, std::forward<Arg>(arg));
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
            this->columnCondition.append(FieldInfo{ field, tbName });
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
     * @tparam Args condition type, EntityField<E> or FunctionConnector
     * @param args fields
     * @return this
     */
    template<typename...Args>
    T& conflictColumns(Args&&... args) {
        this->doColumn(std::forward<Args>(args)...);
        return static_cast<T&>(*this);
    }

    /**
     * use fields or custom function to add upsert conflict columns, using enabled to add column optional
     * @tparam Arg condition type, EntityField<E> or FunctionConnector
     * @param enabled add column if enabled
     * @param arg fields
     * @return this
     */
    template<typename Arg>
    T& conflictColumns(bool enabled, Arg&& arg) {
        this->doColumn(enabled, std::forward<Arg>(arg));
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
     * @tparam Args condition type, EntityField<E> or FunctionConnector
     * @param args fields
     * @return this
     */
    template<typename...Args>
    T& updateColumns(Args&&... args) {
        this->doColumn(std::forward<Args>(args)...);
        return static_cast<T&>(*this);
    }

    /**
     * use fields or custom function to add upsert update columns, using enabled to add column optional
     * @tparam Arg condition type, EntityField<E> or FunctionConnector
     * @param enabled add column if enabled
     * @param arg fields
     * @return this
     */
    template<typename Arg>
    T& updateColumns(bool enabled, Arg&& arg) {
        this->doColumn(enabled, std::forward<Arg>(arg));
        return static_cast<T&>(*this);
    }

    template<template<typename> class, typename>
    friend class BuilderReaderProvider;

    template<template<typename...> class, typename...>
    friend class BuilderJbReaderProvider;
};

QTDAO_END_NAMESPACE
#pragma once

#include "selectimpl.h"

#include "../macro/macro.h"

QTDAO_BEGIN_NAMESPACE

template<typename E>
class Select
        : EntityReaderProvider<E>
        , BuilderReaderProvider<SelectBuilder, E>
        , SelectImpl
{
public:
    /**
     * select top for sqlserver query
     * @param size top size
     * @param percent top percent
     * @return
     */
    Select<E>& top(int size, bool percent = false) {
        topSize = size;
        topPercent = percent;
        return *this;
    }

    /**
     * select a record, report errors when multiple results,
     * if there are no results, return use default value
     * @return E is type of entity
     */
    E unique();

    /**
     * unique select a record, and fetch value by target field
     * @return T type of field value
     */
    template<typename T>
    T unique(const EntityField<T>& field);

    /**
     * unique select a record, and fetch value by target field1 and field2
     * @return QPair<T, K>, T is type of field1 value, K is type of field2 value
     */
    template<typename T, typename K>
    QPair<T, K> unique(const EntityField<T>& field1, const EntityField<K>& field2);

    /**
     * unique select a record, and fetch value by target fields
     * @return std::tuple<T, ...>, multiply type of fields value
     */
    template<typename T, typename... Args>
    std::tuple<T, typename Args::Type...> unique(const EntityField<T>& field, const Args&... args);

    /**
     * select one record,
     * if there are no results, return use default value
     * @return E is type of entity
     */
    E one();

    /**
     * one select a record, and fetch value by target field
     * @return T type of field value
     */
    template<typename T>
    T one(const EntityField<T>& field);

    /**
     * one select a record, and fetch value by target field1 and field2
     * @return QPair<T, K>, T is type of field1 value, K is type of field2 value
     */
    template<typename T, typename K>
    QPair<T, K> one(const EntityField<T>& field1, const EntityField<K>& field2);

    /**
     * one select a record, and fetch value by target fields
     * @return std::tuple<T, ...>, multiply type of fields value
     */
    template<typename T, typename... Args>
    std::tuple<T, typename Args::Type...> one(const EntityField<T>& field, const Args&... args);

    /**
     * read all data select by conditions
     * @return QList<E>, E is type of entity
     */
    QList<E> list();

    /**
     * list select a record, and fetch value by target field
     * @return QList<T>, T is type of field value
     */
    template<typename T>
    QList<T> list(const EntityField<T>& field);

    /**
     * list select a record, and fetch value by target field1 and field2
     * @return QList<QPair<T, K>>, T is type of field1 value, K is type of field2 value
     */
    template<typename T, typename K>
    QList<QPair<T, K>> list(const EntityField<T>& field1, const EntityField<K>& field2);

    /**
     * list select a record, and fetch value by target fields
     * @return QList<std::tuple<T, ...>>, multiply type of fields value
     */
    template<typename T, typename... Args>
    QList<std::tuple<T, typename Args::Type...>> list(const EntityField<T>& field, const Args&... args);

    /**
     * the raw result query, when the result set is large,
     * skips the object transformation and reads the query result directly
     * @param callback
     */
    void raw(const std::function<void(QSqlQuery&)>& callback);

    /**
     * explain query statement
     * @tparam I must one of SqliteExplainInfo/SqliteExplainQueryPlanInfo/MysqlExplainInfo/SqlServerExplainInfo
     * @return SqliteExplainInfo/SqliteExplainQueryPlanInfo/MysqlExplainInfo
     */
    template<typename I>
    QList<I> explain();

private:
    BASE_QUERY_CONSTRUCTOR_DECLARE(Select)

    friend class RecursiveQueryBuilder;
    friend class FunctionCondition;

    template<template<typename> class, typename>
    friend class FromSelfSelectBuilder;

    template<typename T>
    friend class FromE2SelectBuilder;

    template<template<typename...> class, typename...>
    friend class FromEsSelectBuilder;

    template<typename T>
    friend class UnionBuilder;

    template<typename T>
    friend class JoinConnectBuilder;
};

template<typename E>
inline E Select<E>::unique() {

    E entity;
    setDebug(this->builder);
    uniqueExec([&](const QString& fieldName, const QVariant& value){
        EntityReaderProvider<E>::bindValue(entity, fieldName, value);
    });

    return entity;
}

template<typename E>
template<typename T>
inline T Select<E>::unique(const EntityField<T>& field) {
    setDebug(this->builder);
    T d;
    uniqueExec(nullptr, [&](const QSqlRecord& record) {
        d = readFromRecord(record, field);
    });
    return d;
}

template<typename E>
template<typename T, typename K>
inline QPair<T, K> Select<E>::unique(const EntityField<T>& field1, const EntityField<K>& field2) {
    setDebug(this->builder);
    QPair<T, K> d;
    uniqueExec(nullptr, [&](const QSqlRecord& record) {
        d = readFromRecord(record, field1, field2);
    });
    return d;
}

template<typename E>
template<typename T, typename... Args>
inline std::tuple<T, typename Args::Type...> Select<E>::unique(const EntityField<T>& field, const Args&... args) {
    setDebug(this->builder);
    std::tuple<T, typename Args::Type...> d;
    uniqueExec(nullptr, [&](const QSqlRecord& record) {
        d = readFromRecords(record, field, args...);
    });
    return d;
}

template<typename E>
inline E Select<E>::one() {

    E entity;
    setDebug(this->builder);
    oneExec([&](const QString& fieldName, const QVariant& value){
        EntityReaderProvider<E>::bindValue(entity, fieldName, value);
    });

    return entity;
}

template<typename E>
template<typename T>
inline T Select<E>::one(const EntityField<T>& field) {
    setDebug(this->builder);
    T d;
    oneExec(nullptr, [&](const QSqlRecord& record) {
        d = readFromRecord(record, field);
    });
    return d;
}

template<typename E>
template<typename T, typename K>
inline QPair<T, K> Select<E>::one(const EntityField<T>& field1, const EntityField<K>& field2) {
    setDebug(this->builder);
    QPair<T, K> d;
    oneExec(nullptr, [&](const QSqlRecord& record) {
        d = readFromRecord(record, field1, field2);
    });
    return d;
}

template<typename E>
template<typename T, typename... Args>
inline std::tuple<T, typename Args::Type...> Select<E>::one(const EntityField<T>& field, const Args&... args) {
    setDebug(this->builder);
    std::tuple<T, typename Args::Type...> d;
    oneExec(nullptr, [&](const QSqlRecord& record) {
        d = readFromRecords(record, field, args...);
    });
    return d;
}

template<typename E>
inline QList<E> Select<E>::list() {

    QList<E> data;
    setDebug(this->builder);
    listExec([&](const QSqlRecord& record){
        E entity;
        recordBind(record, [&](const QString& fieldName, const QVariant& value){
            EntityReaderProvider<E>::bindValue(entity, fieldName, value);
        });
        data << entity;
    });

    return data;
}

template<typename E>
template<typename T>
inline QList<T> Select<E>::list(const EntityField<T>& field) {
    setDebug(this->builder);
    QList<T> data;
    listExec([&](const QSqlRecord& record){
        data << readFromRecord(record, field);
    });
    return data;
}

template<typename E>
template<typename T, typename K>
inline QList<QPair<T, K>> Select<E>::list(const EntityField<T>& field1, const EntityField<K>& field2) {
    setDebug(this->builder);
    QList<QPair<T, K>> data;
    listExec([&](const QSqlRecord& record){
        data << readFromRecord(record, field1, field2);
    });
    return data;
}

template<typename E>
template<typename T, typename... Args>
inline QList<std::tuple<T, typename Args::Type...>> Select<E>::list(const EntityField<T>& field, const Args&... args) {
    setDebug(this->builder);
    QList<std::tuple<T, typename Args::Type...>> data;
    listExec([&](const QSqlRecord& record){
        data << readFromRecords(record, field, args...);
    });
    return data;
}

template<typename E>
inline void Select<E>::raw(const std::function<void(QSqlQuery&)>& callback) {
    buildFilterSqlStatement();
    setDebug(this->builder);
    auto query = exec();
    callback(query);
}

template<typename E>
template<typename I>
inline QList<I> Select<E>::explain() {
    Q_STATIC_ASSERT_X(ExplainTool<I>::Valid == 1, 
        "template parameter must one of SqliteExplainInfo/SqliteExplainQueryPlanInfo/MysqlExplainInfo/SqlServerExplainInfo");

    return ExplainTool<I>::toExplain(readExplainStatement());
}

QTDAO_END_NAMESPACE
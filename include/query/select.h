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
     * @return
     */
    E unique();

    /**
     * select one record,
     * if there are no results, return use default value
     * @return
     */
    E one();

    /**
     * read all data select by conditions
     * @return
     */
    QList<E> list();

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
inline E Select<E>::one() {

    E entity;
    setDebug(this->builder);
    oneExec([&](const QString& fieldName, const QVariant& value){
        EntityReaderProvider<E>::bindValue(entity, fieldName, value);
    });

    return entity;
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
#pragma once

#include "selectimpl.h"

#include "../macro/macro.h"

QTDAO_BEGIN_NAMESPACE

template<typename E>
class SelectBuilder;

class FunctionCondition;
class RecursiveQueryBuilder;

template<typename E>
class Select : EntityReaderProvider<E>, SelectImpl {
public:
    /// <summary>
    /// select top for sqlserver query
    /// </summary>
    /// <param name="size">top size</param>
    /// <param name="percent">top percent</param>
    /// <returns></returns>
    Select<E>& top(int size, bool percent = false) {
        topSize = size;
        topPercent = percent;
        return *this;
    }

    /// <summary>
    /// select a record, report errors when multiple results 
    /// if there are no results, return use default value
    /// </summary>
    /// <returns></returns>
    E unique();

    /// <summary>
    /// select one record 
    /// if there are no results, return use default value
    /// </summary>
    /// <returns></returns>
    E one();

    /// <summary>
    /// read all data select by conditions
    /// </summary>
    /// <returns></returns>
    QList<E> list();

    /// <summary>
    /// the raw result query, when the result set is large, 
    /// skips the object transformation and reads the query result directly
    /// </summary>
    /// <param name="callback"></param>
    void raw(const std::function<void(QSqlQuery&)>& callback);

    /// <summary>
    /// explain query statement
    /// </summary>
    /// <typeparam name="I">must one of SqliteExplainInfo/SqliteExplainQueryPlanInfo/MysqlExplainInfo/SqlServerExplainInfo</typeparam>
    /// <returns>SqliteExplainInfo/SqliteExplainQueryPlanInfo/MysqlExplainInfo</returns>
    template<typename I>
    QList<I> explain();

private:
    BASE_QUERY_CONSTRUCTOR_DECLARE(Select)

    friend class BaseQueryBuilder;
    friend class RecursiveQueryBuilder;
    friend class FunctionCondition;
};

template<typename E>
inline E Select<E>::unique() {

    E entity;
    uniqueExec([&](const QString& fieldName, const QVariant& value){
        EntityReaderProvider<E>::bindValue(entity, fieldName, value);
    });

    return entity;
}

template<typename E>
inline E Select<E>::one() {

    E entity;
    oneExec([&](const QString& fieldName, const QVariant& value){
        EntityReaderProvider<E>::bindValue(entity, fieldName, value);
    });

    return entity;
}

template<typename E>
inline QList<E> Select<E>::list() {

    QList<E> data;
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
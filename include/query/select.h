#pragma once

#include "basequery.h"

#include "../macro/macro.h"

#include "../dbloader.h"

template<typename E>
class SelectBuilder;

class FunctionCondition;
class RecursiveQueryBuilder;

template<typename E>
class Select : BaseQuery {
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
    void raw(std::function<void(QSqlQuery&)> callback);

    /// <summary>
    /// explain query statement
    /// </summary>
    /// <typeparam name="I">must one of SqliteExplainInfo/SqliteExplainQueryPlanInfo/MysqlExplainInfo/SqlServerExplainInfo</typeparam>
    /// <returns>SqliteExplainInfo/SqliteExplainQueryPlanInfo/MysqlExplainInfo</returns>
    template<typename I>
    QList<I> explain();

protected:
    void buildFilterSqlStatement();

protected:
    QString getBindColumns(QVariantList& values);

    BASE_QUERY_CONSTRUCTOR_DECLARE(Select)

    friend class BaseQueryBuilder;
    friend class RecursiveQueryBuilder;
    friend class FunctionCondition;

private:
    int topSize = 0;
    bool topPercent;
};

template<typename E>
inline E Select<E>::unique() {
    buildFilterSqlStatement();
    E entity;
    exec([&](QSqlQuery& query) {
        int resultSize = 0;
        while (query.next()) {
            typename E::Tool tool;
            QSqlRecord record = query.record();
            for (int i = 0; i < record.count(); i++) {
                tool.bindValue(entity, record.fieldName(i), record.value(i));
            }
            resultSize++;
        }
        if (resultSize > 1) {
            printException("unique result size > 1, actual is " + QString::number(resultSize));
        }
    });
    return entity;
}

template<typename E>
inline E Select<E>::one() {
    buildFilterSqlStatement();
    E entity;
    exec([&](QSqlQuery& query) {
        if (query.next()) {
            typename E::Tool tool;
            QSqlRecord record = query.record();
            for (int i = 0; i < record.count(); i++) {
                tool.bindValue(entity, record.fieldName(i), record.value(i));
            }
        }
    });
    return entity;
}

template<typename E>
inline QList<E> Select<E>::list() {
    buildFilterSqlStatement();
    QList<E> data;
    exec([&](QSqlQuery& query) {
        typename E::Tool tool;
        while (query.next()) {
            E entity;
            QSqlRecord record = query.record();
            for (int i = 0; i < record.count(); i++) {
                tool.bindValue(entity, record.fieldName(i), record.value(i));
            }
            data << entity;
        }
    });
    return data;
}

template<typename E>
inline void Select<E>::raw(std::function<void(QSqlQuery&)> callback) {
    buildFilterSqlStatement();
    exec(callback);
}

template<typename E>
inline void Select<E>::buildFilterSqlStatement() {
    QString sql = "select %1%2 from %3";
    typename E::Info info;
    QVariantList values;

    if (topSize != 0 && DbLoader::getConfig().isSqlServer()) {
        sql = sql.arg("top " + QString::number(topSize) + (topPercent ? " percent " : " "));
    } else {
        sql = sql.arg("");
    }
    sql = sql.arg(getBindColumns(values));
    if (builder->fromSelectStatement.isEmpty()) {
        sql = sql.arg(info.getTableName());
    } else {
        if (builder->recursiveQuery) {
            sql = sql.arg(builder->fromSelectAs);
            sql = builder->fromSelectStatement + sql;
            builder->fromSelectValues.append(values);
            builder->fromSelectValues.swap(values);
        } else {
            sql = sql.arg('(' + builder->fromSelectStatement + ") as " + builder->fromSelectAs);
            values.append(builder->fromSelectValues);
        }
    }

    if (!builder->filterCondition.isEmpty()) {
        builder->filterCondition.connect();
        sql.append(" where ").append(builder->filterCondition.getConditionStr());
        values.append(builder->filterCondition.getValues());
    }

    if (!builder->constraintCondition.isEmpty()) {
        builder->constraintCondition.connect();
        sql.append(" ").append(builder->constraintCondition.getConditionStr());
        values.append(builder->constraintCondition.getValues());
    }

    if (!builder->unionSelectStatement.isEmpty()) {
        sql.append(builder->unionAll ? " union all " : " union ");
        sql.append(builder->unionSelectStatement);
        values.append(builder->unionSelectValues);
    }

    setSqlQueryStatement(sql, values);
}

template<typename E>
inline QString Select<E>::getBindColumns(QVariantList& values) {
    if (builder->columnBind.isEmpty()) {
        return "*";
    }
    builder->columnBind.connect();
    values << builder->columnBind.getValues();
    return builder->columnBind.getConditionStr();
}

template<typename E>
template<typename I>
inline QList<I> Select<E>::explain() {
    Q_STATIC_ASSERT_X(ExplainTool<I>::Valid == 1, 
        "template parameter must one of SqliteExplainInfo/SqliteExplainQueryPlanInfo/MysqlExplainInfo/SqlServerExplainInfo");

    buildFilterSqlStatement();
    auto newStatement = DbLoader::getClient().translateSqlStatement(statement, values);
    return ExplainTool<I>::toExplain(newStatement);
}

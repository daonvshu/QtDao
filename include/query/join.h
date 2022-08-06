#pragma once

#include "basequery.h"

#include <QVector>
#include <QPair>

#include "../dbloader.h"

template<typename... E>
class JoinBuilder;

class RecursiveQueryBuilder;
class FunctionCondition;

enum JoinType {
    CrossJoin,
    InnerJoin,
    LeftJoin,
    RightJoin,
    FullJoin,
};

struct JoinData {
    JoinType joinType;
    Connector filter;
    //select from select
    QString fromSelectStatement;
    QVariantList fromSelectValues;
    QString fromSelectAs;
    bool recursiveQuery;
};

template<typename... E>
class Join : BaseQuery {
public:
    /// <summary>
    /// get select result list
    /// </summary>
    /// <returns></returns>
    QList<std::tuple<E...>> list();

    /// <summary>
    /// explain query statement
    /// </summary>
    /// <typeparam name="I">must one of SqliteExplainInfo/SqliteExplainQueryPlanInfo/MysqlExplainInfo/SqlServerExplainInfo</typeparam>
    /// <returns>SqliteExplainInfo/SqliteExplainQueryPlanInfo/MysqlExplainInfo</returns>
    template<typename I>
    QList<I> explain();

private:
    JoinData mainData;
    QString mainTable;
    QHash<QString, JoinData> subJoinData;
    QList<FieldInfo> usedColumns;

    QHash<QString, QString> tableOrder;
    QList<QPair<QString, QString>> sequenceTableNames;

    bool insideRecursiveQuery = false;

    friend class JoinBuilder<E...>;
    Join(bool throwable, JoinBuilder<E...>* builder) : BaseQuery(throwable, builder) {}

private:
    void buildJoinSqlStatement();

    void setTableOrder();
    QString getAllEntityField();
    QString getJoinTypeName(JoinType type);
    void initSequenceTbName();

    void resultBind(std::tuple<E...>& result, QSqlQuery& query);

    friend class BaseQueryBuilder;
    friend class RecursiveQueryBuilder;
    friend class FunctionCondition;
};

template<typename ...E>
inline QList<std::tuple<E...>> Join<E...>::list() {
    buildJoinSqlStatement();
    QList<std::tuple<E...>> results;
    exec([&](QSqlQuery& query) {
        while (query.next()) {
            std::tuple<E...> result;
            resultBind(result, query);
            results << result;
        }
    });
    return results;
}

template<typename ...E>
inline void Join<E...>::buildJoinSqlStatement() {
    //select (columns) from (main/select/join) a (jointype) (table/select/join) (joinfilter) (mainfilter) (main constrain)
    QString sql = "select ";
    QVariantList values;

    setTableOrder();
    auto prefixGetter = [&](const QString& tb) {
        return tableOrder.value(tb);
    };
    usedColumns.clear();
    if (!builder->columnBind.isEmpty()) {
        builder->columnBind.connect(prefixGetter);
        sql.append(builder->columnBind.getConditionStr());
        values.append(builder->columnBind.getValues());
        usedColumns = builder->columnBind.getUsedFieldNames();
    } else {
        sql.append(getAllEntityField());
    }
    sql.append(" from ");
    if (mainData.fromSelectStatement.isEmpty()) {
        sql.append(mainTable);
    } else {
        if (mainData.recursiveQuery) {
            sql.prepend(mainData.fromSelectStatement).append(mainData.fromSelectAs);
        } else {
            sql.append("(").append(mainData.fromSelectStatement).append(")");
        }
        values.append(mainData.fromSelectValues);
    }
    sql.append(' ').append(prefixGetter(mainTable));
    sql.chop(1);

    initSequenceTbName();
    for (const auto& tb : sequenceTableNames) {
        if (tb.first == mainTable) {
            continue;
        }
        auto joinData = subJoinData.take(tb.first);
        sql.append(' ');
        sql.append(getJoinTypeName(joinData.joinType));
        sql.append(' ');
        if (joinData.fromSelectStatement.isEmpty()) {
            sql.append(insideRecursiveQuery ? tb.first : tb.second);
        } else {
            if (joinData.recursiveQuery) {
                sql.prepend(joinData.fromSelectStatement).append(joinData.fromSelectAs);
                joinData.fromSelectValues.append(values);
                joinData.fromSelectValues.swap(values);
            } else {
                sql.append("(").append(joinData.fromSelectStatement).append(")");
                values.append(joinData.fromSelectValues);
            }
        }
        sql.append(' ').append(prefixGetter(tb.first));
        sql.chop(1);
        if (!joinData.filter.isEmpty()) {
            joinData.filter.connect(prefixGetter);
            sql.append(" on ").append(joinData.filter.getConditionStr());
            values.append(joinData.filter.getValues());
        }
    }
    if (!mainData.filter.isEmpty()) {
        mainData.filter.connect(prefixGetter);
        sql.append(" where ").append(mainData.filter.getConditionStr());
        values.append(mainData.filter.getValues());
    }
    bool unionSelect = !builder->unionSelectStatement.isEmpty();
    if (unionSelect) {
        sql.append(builder->unionAll ? " union all " : " union ");
        sql.append(builder->unionSelectStatement);
        values.append(builder->unionSelectValues);
    }

    if (!builder->constraintCondition.isEmpty()) {
        builder->constraintCondition.connect(
            unionSelect && !DbLoader::getConfig().isSqlite() ? std::function<QString(const QString&)>() : prefixGetter
        );
        sql.append(' ');
        sql.append(builder->constraintCondition.getConditionStr());
        values.append(builder->constraintCondition.getValues());
    }
    
    setSqlQueryStatement(sql, values);
}

template<typename... T> struct JoinEUnpackHelper;
template<typename E, typename... T>
struct JoinEUnpackHelper<E, T...> : JoinEUnpackHelper<T...> {
    static void setOrder(QHash<QString, QString>& orderMap, int i) {
        orderMap.insert(E::Info::getTableName(), ('a' + i) + QString("."));
        JoinEUnpackHelper<T...>::setOrder(orderMap, i + 1);
    }

    static QString readEntityFields(const QHash<QString, QString>& orderMap, QList<FieldInfo>& fieldInfo) {
        typename E::Info info;
        QStringList fields = info.getFields();
        auto tbName = info.getTableName();
        QString str;
        for (const auto& f : fields) {
            str.append(orderMap.value(tbName)).append(f).append(",");
            fieldInfo.append(FieldInfo{ f,tbName });
        }
        str.append(JoinEUnpackHelper<T...>::readEntityFields(orderMap, fieldInfo));
        return str;
    }

    static QList<QPair<QString, QString>> getTbName() {
        typename E::Info info;
        QList<QPair<QString, QString>> names;
        names << qMakePair(info.getTableName(), info.getSourceName());
        names.append(JoinEUnpackHelper<T...>::getTbName());
        return names;
    }

    template<int I, typename... K>
    static void bindTupleValue(std::tuple<K...>& result, const QVector<QList<QPair<QString, QVariant>>>& values) {
        typename E::Tool tool;
        const auto& value = values.at(I);
        for (const auto& v : value) {
            tool.bindValue(std::get<I>(result), v.first, v.second);
        }
        JoinEUnpackHelper<T...>::template bindTupleValue<I + 1, K...>(result, values);
    }
};
template<> struct JoinEUnpackHelper<> {
    static void setOrder(QHash<QString, QString>& orderMap, int i) {Q_UNUSED(orderMap);Q_UNUSED(i);}
    static QString readEntityFields(const QHash<QString, QString>& orderMap, QList<FieldInfo>& fieldInfo) { Q_UNUSED(orderMap); Q_UNUSED(fieldInfo); return QString(); }
    static QList<QPair<QString, QString>> getTbName() { return QList<QPair<QString, QString>>(); }

    template<int I, typename... K>
    static void bindTupleValue(std::tuple<K...>& result, const QVector<QList<QPair<QString, QVariant>>>& values) {Q_UNUSED(result);Q_UNUSED(values);}
};

template<typename ...E>
inline void Join<E...>::setTableOrder() {
    tableOrder.clear();
    JoinEUnpackHelper<E...>::setOrder(tableOrder, 0);
}

template<typename ...E>
inline QString Join<E...>::getAllEntityField() {
    return JoinEUnpackHelper<E...>::readEntityFields(tableOrder, usedColumns).chopped(1);
}

template<typename ...E>
inline QString Join<E...>::getJoinTypeName(JoinType type) {
    switch (type) {
    case CrossJoin:
        return "cross join";
    case InnerJoin:
        return "inner join";
    case LeftJoin:
        return "left join";
    case RightJoin:
        return "right join";
    case FullJoin:
        return "full join";
    }
    return QString();
}

template<typename ...E>
inline void Join<E...>::initSequenceTbName() {
    sequenceTableNames = JoinEUnpackHelper<E...>::getTbName();
}

template<typename ...E>
inline void Join<E...>::resultBind(std::tuple<E...>& result, QSqlQuery& query) {
    QVector<QList<QPair<QString, QVariant>>> resultValues(sequenceTableNames.size());
    for (int i = 0; i < usedColumns.size(); i++) {
        auto tb = usedColumns.at(i).bindTable;
        int tbIndex = 0;
        for (int j = 0; j < sequenceTableNames.size(); j++) {
            if (tb == sequenceTableNames.at(j).first) {
                tbIndex = j;
                break;
            }
        }
        resultValues[tbIndex] << qMakePair(usedColumns.at(i).name, query.value(i));
    }
    JoinEUnpackHelper<E...>::template bindTupleValue<0, E...>(result, resultValues);
}

template<typename ...E>
template<typename I>
inline QList<I> Join<E...>::explain() {
    Q_STATIC_ASSERT_X(ExplainTool<I>::Valid == 1,
        "template parameter must one of SqliteExplainInfo/SqliteExplainQueryPlanInfo/MysqlExplainInfo/SqlServerExplainInfo");

    buildJoinSqlStatement();
    auto newStatement = DbLoader::getClient().translateSqlStatement(statement, values);
    return ExplainTool<I>::toExplain(newStatement);
}

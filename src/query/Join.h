﻿#pragma once

#include "BaseQuery.h"

template<typename... T>
class JoinBuilder;

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
};

template<typename... E>
class Join : BaseQuery {
public:
    QList<std::tuple<E...>> list();

private:
    JoinData mainData;
    QString mainTable;
    QHash<QString, JoinData> subJoinData;
    Connector columnBind, constraintCondition;
    QList<FieldInfo> usedColumns;
    QStringList sequenceTableNames;

    QHash<QString, QString> tableOrder;

    friend class JoinBuilder<E...>;

private:
    void buildJoinSqlStatement();

    void setTableOrder();
    QString getAllEntityField();
    QString getJoinTypeName(JoinType type);
    QStringList getSequenceTbName();

    QVector<QStringList> groupUsedColumnsByTableOrder();

    void resultBind(std::tuple<E...>& result, QSqlQuery& query);
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
    //select (columns) from (main) a (jointype) (joinfilter) (mainfilter) (main constrain)
    QString sql = "select ";
    QVariantList values;

    setTableOrder();
    auto prefixGetter = [&](const QString& tb) {
        return tableOrder.value(tb);
    };
    usedColumns.clear();
    if (!columnBind.isEmpty()) {
        columnBind.connect(prefixGetter);
        sql.append(columnBind.getConditionStr());
        values.append(columnBind.getValues());
        usedColumns = columnBind.getUsedFieldNames();
    } else {
        sql.append(getAllEntityField());
    }
    sql.append(" from ");
    sql.append(mainTable).append(' ').append(prefixGetter(mainTable));
    sql.chop(1);

    sequenceTableNames = getSequenceTbName();
    for (const auto& tb : sequenceTableNames) {
        if (tb == mainTable) {
            continue;
        }
        auto joinData = subJoinData.value(tb);
        sql.append(' ');
        sql.append(getJoinTypeName(joinData.joinType));
        sql.append(' ');
        sql.append(tb).append(' ').append(prefixGetter(tb));
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
    if (!constraintCondition.isEmpty()) {
        constraintCondition.connect(prefixGetter);
        sql.append(' ');
        sql.append(constraintCondition.getConditionStr());
        values.append(constraintCondition.getValues());
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

    static QStringList getTbName() {
        typename E::Info info;
        QStringList names;
        names.append(info.getTableName());
        names.append(JoinEUnpackHelper<T...>::getTbName());
        return names;
    }

    template<int I, typename... K>
    static void bindTupleValue(std::tuple<K...>& result, const QVector<QList<std::tuple<QString, QVariant>>>& values) {
        typename E::Tool tool;
        const auto& value = values.at(I);
        for (const auto& v : value) {
            tool.bindValue(std::get<I>(result), std::get<0>(v), std::get<1>(v));
        }
        JoinEUnpackHelper<T...>::bindTupleValue<I + 1, K...>(result, values);
    }
};
template<> struct JoinEUnpackHelper<> {
    static void setOrder(QHash<QString, QString>& orderMap, int i) {}
    static QString readEntityFields(const QHash<QString, QString>& orderMap, QList<FieldInfo>& fieldInfo) { return QString(); }
    static QStringList getTbName() { return QStringList(); }

    template<int I, typename... K>
    static void bindTupleValue(std::tuple<K...>& result, const QVector<QList<std::tuple<QString, QVariant>>>& values) {}
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
    default:
        break;
    }
    return QString();
}

template<typename ...E>
inline QStringList Join<E...>::getSequenceTbName() {
    return JoinEUnpackHelper<E...>::getTbName();
}

template<typename ...E>
inline QVector<QStringList> Join<E...>::groupUsedColumnsByTableOrder() {
    QVector<QStringList> results;
    auto tables = getSequenceTbName();
    results.resize(tables.size());
    for (const auto& col : usedColumns) {
        int colIndex = qMax(0, tables.indexOf(col.bindTable));
        results[colIndex].append(col.name);
    }
    return results;
}

template<typename ...E>
inline void Join<E...>::resultBind(std::tuple<E...>& result, QSqlQuery& query) {
    QVector<QList<std::tuple<QString, QVariant>>> resultValues(sequenceTableNames.size());
    for (int i = 0; i < usedColumns.size(); i++) {
        auto tb = usedColumns.at(i).bindTable;
        int tbIndex = sequenceTableNames.indexOf(tb);
        resultValues[tbIndex] << std::make_tuple(usedColumns.at(i).name, query.value(i));
    }
    JoinEUnpackHelper<E...>::bindTupleValue<0, E...>(result, resultValues);
}
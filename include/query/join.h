#pragma once

#include "joinimpl.h"

#include <QVector>
#include <QPair>

QTDAO_BEGIN_NAMESPACE

template<typename... E>
class Join
        : JoinImpl
        , BuilderJbReaderProvider<JoinBuilder, E...>
{
public:
    /**
     * get select result list
     * @return
     */
    QList<std::tuple<E...>> list();

    /**
     * explain query statement
     * @tparam I must one of SqliteExplainInfo/SqliteExplainQueryPlanInfo/MysqlExplainInfo/SqlServerExplainInfo
     * @return
     */
    template<typename I>
    QList<I> explain();

private:
    friend class JoinBuilder<E...>;

    Join(const QString& mainTable, const QHash<QString, JoinData>& subJoinData, JoinBuilder<E...> builder)
        : JoinImpl(mainTable, subJoinData)
        , BuilderJbReaderProvider<JoinBuilder, E...>(std::move(builder))
    {}

private:
    void setTableOrder() override;

    QString getAllEntityField() override;

    QString getJoinTypeName(JoinType type) override;

    void initSequenceTbName() override;

    void resultBind(std::tuple<E...>& result, QSqlQuery& query);

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

template<typename ...E>
inline QList<std::tuple<E...>> Join<E...>::list() {
    buildJoinSqlStatement();
    QList<std::tuple<E...>> results;
    setDebug(this->builder);
    auto query = exec();
    while (query.next()) {
        std::tuple<E...> result;
        resultBind(result, query);
        results << result;
    }
    return results;
}

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
        case JoinType::CrossJoin:
            return "cross join";
        case JoinType::InnerJoin:
            return "inner join";
        case JoinType::LeftJoin:
            return "left join";
        case JoinType::RightJoin:
            return "right join";
        case JoinType::FullJoin:
            return "full join";
        case JoinType::Unset:
            qFatal("sub table join type unset!");
            break;
    }
    return {};
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

    return ExplainTool<I>::toExplain(readExplainStatement());
}

QTDAO_END_NAMESPACE
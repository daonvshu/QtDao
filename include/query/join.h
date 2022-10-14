#pragma once

#include "joinimpl.h"

#include <QVector>
#include <QPair>

QTDAO_BEGIN_NAMESPACE

template<typename... E>
class JoinBuilder;

class RecursiveQueryBuilder;
class FunctionCondition;

template<typename... E>
class Join : JoinImpl {
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
    friend class JoinBuilder<E...>;
    Join(bool fatalEnabled, JoinBuilder<E...>* builder) : JoinImpl(fatalEnabled, builder) {}

protected:


private:
    void setTableOrder() override;

    QString getAllEntityField() override;

    QString getJoinTypeName(JoinType type) override;

    void initSequenceTbName() override;

    void resultBind(std::tuple<E...>& result, QSqlQuery& query);

    friend class BaseQueryBuilder;
    friend class RecursiveQueryBuilder;
    friend class FunctionCondition;
};

template<typename ...E>
inline QList<std::tuple<E...>> Join<E...>::list() {
    buildJoinSqlStatement();
    QList<std::tuple<E...>> results;
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
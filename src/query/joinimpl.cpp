#include "query/joinimpl.h"
#include "dao.h"

QTDAO_BEGIN_NAMESPACE

void JoinImpl::buildJoinSqlStatement() {
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
            unionSelect && !globalConfig->isSqlite() ? std::function<QString(const QString&)>() : prefixGetter
        );
        sql.append(' ');
        sql.append(builder->constraintCondition.getConditionStr());
        values.append(builder->constraintCondition.getValues());
    }

    setSqlQueryStatement(sql, values);
}

QString JoinImpl::readExplainStatement() {
    buildJoinSqlStatement();
    return globalConfig->getClient()->translateSqlStatement(statement, values);
}

QTDAO_END_NAMESPACE

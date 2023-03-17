#include "query/joinimpl.h"
#include "config/configbuilder.h"

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
    auto& cc = columnConnector();
    if (!cc.isEmpty()) {
        cc.connect(prefixGetter);
        sql.append(cc.getConditionStr());
        values.append(cc.getValues());
        usedColumns = cc.getUsedFieldNames();
    } else {
        sql.append(getAllEntityField());
    }
    sql.append(" from ");
    auto& mainFromData = fromBuildData();
    if (mainFromData.statement.isEmpty()) {
        sql.append(mainTable);
    } else {
        if (mainFromData.recursiveQuery) {
            sql.prepend(mainFromData.statement).append(mainFromData.asName);
        } else {
            sql.append("(").append(mainFromData.statement).append(")");
        }
        values.append(mainFromData.values);
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
        if (joinData.fromBuildData.statement.isEmpty()) {
            sql.append(insideRecursiveQuery ? tb.first : tb.second);
        } else {
            if (joinData.fromBuildData.recursiveQuery) {
                sql.prepend(joinData.fromBuildData.statement).append(joinData.fromBuildData.asName);
                joinData.fromBuildData.values.append(values);
                joinData.fromBuildData.values.swap(values);
            } else {
                sql.append("(").append(joinData.fromBuildData.statement).append(")");
                values.append(joinData.fromBuildData.values);
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

    auto& fc = filterConnector();
    if (!fc.isEmpty()) {
        fc.connect(prefixGetter);
        sql.append(" where ").append(fc.getConditionStr());
        values.append(fc.getValues());
    }

    auto& unionData = unionBuildData();
    bool unionSelect = !unionData.statement.isEmpty();
    if (unionSelect) {
        sql.append(unionData.unionAll ? " union all " : " union ");
        sql.append(unionData.statement);
        values.append(unionData.values);
    }

    auto& constraint = constraintConnector();
    if (!constraint.isEmpty()) {
        constraint.connect(
            unionSelect && !globalConfig->isSqlite() ? std::function<QString(const QString&)>() : prefixGetter
        );
        sql.append(' ');
        sql.append(constraint.getConditionStr());
        values.append(constraint.getValues());
    }

    setSqlQueryStatement(sql, values);
}

QString JoinImpl::readExplainStatement() {
    buildJoinSqlStatement();
    return globalConfig->getClient()->translateSqlStatement(statement, values);
}

QTDAO_END_NAMESPACE

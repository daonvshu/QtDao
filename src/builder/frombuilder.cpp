#include "builder/option/frombuilder.h"

#include "query/selectimpl.h"
#include "query/joinimpl.h"
#include "builder/recursivequerybuilder.h"

QTDAO_BEGIN_NAMESPACE

void FromBuilder::fromSelect(SelectImpl &select) {
    select.buildFilterSqlStatement();
    fromData.statement = select.statement;
    fromData.values = select.values;

    auto& selectFromData = select.fromBuildData();
    if (selectFromData.asName.isEmpty()) {
        fromData.asName = "sel_" + select.getTableName();
    } else {
        fromData.asName = "sel_" + selectFromData.asName;
    }
}

void FromBuilder::fromJoin(JoinImpl &join) {
    join.buildJoinSqlStatement();
    fromData.statement = join.statement;
    fromData.values = join.values;

    auto& mainFromData = join.fromBuildData();
    if (mainFromData.asName.isEmpty()) {
        fromData.asName = "join_" + join.mainTable;
    } else {
        fromData.asName = "join_" + mainFromData.asName;
    }
}

void FromBuilder::fromBuilder(RecursiveQueryBuilder &builder) {
    Q_ASSERT(!builder.initialQueryStatement.isEmpty());
    Q_ASSERT(!builder.recursiveQueryStatement.isEmpty());
    Q_ASSERT(!builder.tmpTableName.isEmpty());

    fromData.asName = builder.tmpTableName;
    fromData.statement = QString("with %1 as (%2 %3 %4) ")
            .arg(fromData.asName,
                 builder.initialQueryStatement,
                 builder.unionAll ? "union all" : "union",
                 builder.recursiveQueryStatement
                 );
    fromData.values.append(builder.initialQueryValue);
    fromData.values.append(builder.recursiveQueryValue);
    fromData.recursiveQuery = true;
}

QTDAO_END_NAMESPACE


#include "builder/option/frombuilder.h"

#include "query/selectimpl.h"
#include "query/joinimpl.h"
#include "builder/recursivequerybuilder.h"

QTDAO_BEGIN_NAMESPACE

void FromBuilder::from(SelectImpl &select) {
    select.buildFilterSqlStatement();
    data.statement = select.statement;
    data.value = select.values;
    if (select.builder->fromSelectAs.isEmpty()) {
        fromSelectAs = "sel_" + E::Info::getTableName();
    } else {
        fromSelectAs = "sel_" + select.builder->fromSelectAs;
    }
}

void FromBuilder::from(JoinImpl &join) {
    join.buildJoinSqlStatement();
    fromSelectStatement = join.statement;
    fromSelectValues = join.values;
    if (join.builder->fromSelectAs.isEmpty()) {
        fromSelectAs = "join_" + join.mainTable;
    } else {
        fromSelectAs = "join_" + join.builder->fromSelectAs;
    }
}

void FromBuilder::fromBuilder(RecursiveQueryBuilder &builder) {
    Q_ASSERT(!builder.initialQueryStatement.isEmpty());
    Q_ASSERT(!builder.recursiveQueryStatement.isEmpty());
    Q_ASSERT(!builder.tmpTableName.isEmpty());

    fromSelectAs = builder.tmpTableName;
    fromSelectStatement = QString("with %1 as (%2 %3 %4) ")
            .arg(fromSelectAs)
            .arg(builder.initialQueryStatement)
            .arg(builder.unionAll ? "union all" : "union")
            .arg(builder.recursiveQueryStatement)
            ;
    fromSelectValues.append(builder.initialQueryValue);
    fromSelectValues.append(builder.recursiveQueryValue);
    recursiveQuery = true;
}

QTDAO_END_NAMESPACE


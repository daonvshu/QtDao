#include "BaseQueryBuilder.h"

#include "RecursiveQueryBuilder.h"

void BaseQueryBuilder::from(RecursiveQueryBuilder& builder) {
    Q_ASSERT(!builder.initialQueryStatement.isEmpty());
    Q_ASSERT(!builder.recursiveQueryStatement.isEmpty());
    Q_ASSERT(!builder.tmpTableName.isEmpty());

    fromSelectAs = builder.tmpTableName;
    fromSelectStatement = QString("with recursive %1 as (%2 %3 %4) ")
        .arg(fromSelectAs)
        .arg(builder.initialQueryStatement)
        .arg(builder.unionAll ? "union all" : "union")
        .arg(builder.recursiveQueryStatement)
        ;
    fromSelectValues.append(builder.initialQueryValue);
    fromSelectValues.append(builder.recursiveQueryValue);
    recursiveQuery = true;
}

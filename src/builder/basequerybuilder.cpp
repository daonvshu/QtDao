#include "builder/basequerybuilder.h"

#include "builder/recursivequerybuilder.h"

QTDAO_BEGIN_NAMESPACE

BaseQueryBuilder::~BaseQueryBuilder() {
}

void BaseQueryBuilder::set() {
}

void BaseQueryBuilder::filter() {
}

void BaseQueryBuilder::with() {
}

void BaseQueryBuilder::column() {
}

void BaseQueryBuilder::on() {
}

void BaseQueryBuilder::from(RecursiveQueryBuilder& builder) {
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

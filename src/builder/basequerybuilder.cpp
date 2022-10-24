#include "builder/basequerybuilder.h"

#include "builder/recursivequerybuilder.h"

QTDAO_BEGIN_NAMESPACE

BaseQueryBuilder::~BaseQueryBuilder() {
}

void BaseQueryBuilder::set(bool enabled, const EntityCondition &condition) {
    if (enabled) {
        setCondition.append(condition);
    }
}

void BaseQueryBuilder::set() {
}

void BaseQueryBuilder::filter(bool enabled, const EntityCondition &condition) {
    if (enabled) {
        filterCondition.append(condition);
    }
}

void BaseQueryBuilder::filter(bool enabled, const Connector &condition) {
    if (enabled) {
        filterCondition.append(condition);
    }
}

void BaseQueryBuilder::filter(bool enabled, const FunctionCondition &condition) {
    if (enabled) {
        filterCondition.append(condition);
    }
}

void BaseQueryBuilder::filter() {
}

void BaseQueryBuilder::on(bool enabled, const EntityCondition &condition) {
    if (enabled) {
        onCondition.append(condition);
    }
}

void BaseQueryBuilder::on(bool enabled, const Connector &condition) {
    if (enabled) {
        onCondition.append(condition);
    }
}

void BaseQueryBuilder::on(bool enabled, const FunctionCondition &condition) {
    if (enabled) {
        onCondition.append(condition);
    }
}

void BaseQueryBuilder::on() {
}

void BaseQueryBuilder::with(bool enabled, const dao::ConditionConstraint &constraint) {
    if (enabled) {
        constraintCondition.append(constraint);
    }
}

void BaseQueryBuilder::with() {
}

void BaseQueryBuilder::column() {
}

void BaseQueryBuilder::conflictColumns() {
}

void BaseQueryBuilder::updateColumns() {
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

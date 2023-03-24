#pragma once

#include "frombuilder.h"

QTDAO_BEGIN_NAMESPACE

template<typename T>
class JoinConnectBuilder : protected FromBuilder {
public:
    template<typename E>
    T& crossJoin() {
        return connectJoin<E, JoinType::CrossJoin>();
    }

    template<typename E>
    T& crossJoin(Select<E>& select) {
        return connectJoin<E, JoinType::CrossJoin>(select);
    }

    template<typename E>
    T& crossJoin(Select<E>&& select) {
        return crossJoin(select);
    }

    T& crossJoin(RecursiveQueryBuilder& builder) {
        return connectJoin<JoinType::CrossJoin>(builder);
    }

    T& crossJoin(RecursiveQueryBuilder&& builder) {
        return crossJoin(builder);
    }

    template<typename E>
    T& innerJoin() {
        return connectJoin<E, JoinType::InnerJoin>();
    }

    template<typename E>
    T& innerJoin(Select<E>& select) {
        return connectJoin<E, JoinType::InnerJoin>(select);
    }

    template<typename E>
    T& innerJoin(Select<E>&& select) {
        return innerJoin(select);
    }

    T& innerJoin(RecursiveQueryBuilder& builder) {
        return connectJoin<JoinType::InnerJoin>(builder);
    }

    T& innerJoin(RecursiveQueryBuilder&& builder) {
        return innerJoin(builder);
    }

    template<typename E>
    T& leftJoin() {
        return connectJoin<E, JoinType::LeftJoin>();
    }

    template<typename E>
    T& leftJoin(Select<E>& select) {
        return connectJoin<E, JoinType::LeftJoin>(select);
    }

    template<typename E>
    T& leftJoin(Select<E>&& select) {
        return leftJoin(select);
    }

    T& leftJoin(RecursiveQueryBuilder& builder) {
        return connectJoin<JoinType::LeftJoin>(builder);
    }

    T& leftJoin(RecursiveQueryBuilder&& builder) {
        return leftJoin(builder);
    }

    template<typename E>
    T& rightJoin() {
        return connectJoin<E, JoinType::RightJoin>();
    }

    template<typename E>
    T& rightJoin(Select<E>& select) {
        return connectJoin<E, JoinType::RightJoin>(select);
    }

    template<typename E>
    T& rightJoin(Select<E>&& select) {
        return rightJoin(select);
    }

    T& rightJoin(RecursiveQueryBuilder& builder) {
        return connectJoin<JoinType::RightJoin>(builder);
    }

    T& rightJoin(RecursiveQueryBuilder&& builder) {
        return rightJoin(builder);
    }

    template<typename E>
    T& fullJoin() {
        return connectJoin<E, JoinType::FullJoin>();
    }

    template<typename E>
    T& fullJoin(Select<E>& select) {
        return connectJoin<E, JoinType::FullJoin>(select);
    }

    template<typename E>
    T& fullJoin(Select<E>&& select) {
        return fullJoin(select);
    }

    T& fullJoin(RecursiveQueryBuilder& builder) {
        return connectJoin<JoinType::FullJoin>(builder);
    }

    T& fullJoin(RecursiveQueryBuilder&& builder) {
        return fullJoin(builder);
    }

    virtual void solveLastJoinData() = 0;

private:
    template<typename E, JoinType JT>
    T& connectJoin() {
        solveLastJoinData();
        joinType = JT;
        tbName = E::Info::getTableName();
        return static_cast<T&>(*this);
    }

    template<typename E, JoinType JT>
    T& connectJoin(Select<E>& select) {
        solveLastJoinData();
        fromSelect(static_cast<SelectImpl&>(select));
        return connectJoin<E, JT>();
    }

    template<JoinType JT>
    T& connectJoin(RecursiveQueryBuilder& builder) {
        solveLastJoinData();
        fromBuilder(builder);
        joinType = JT;
        tbName = fromData.asName;
        return static_cast<T&>(*this);
    }

protected:
    using FromBuilder::fromDataClear;

protected:
    JoinType joinType = JoinType::Unset;
    QString tbName;
};

QTDAO_END_NAMESPACE
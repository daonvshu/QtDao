#pragma once

#include "../condition/EntityCondition.h"
#include "../condition/Connector.h"
#include "../condition/FunctionCondition.h"
#include "../condition/EntityField.h"

#include "../query/Select.h"
#include "../query/Join.h"

#include "RecursiveQueryBuilder.h"

template<typename E> class Insert;
template<typename E> class Select;
template<typename E> class Update;
template<typename E> class Delete;
template<typename... E> class Join;
template<typename E> class InsertIntoSelect;

class BaseQueryBuilder {
public:
    BaseQueryBuilder() 
        : setThrowable(false)
        , setCondition(",")
        , columnBind(",")
        , filterCondition("and")
        , onCondition("and")
        , recursiveQuery(false)
    {
    }

    ~BaseQueryBuilder() {}

protected:
    template<typename... Args>
    void set(const EntityCondition& condition, const Args&... args);

    template<typename... Args>
    void filter(const EntityCondition& condition, const Args&... args);

    template<typename... Args>
    void filter(const Connector& condition, const Args&... args);

    template<typename... Args>
    void filter(const FunctionCondition& condition, const Args&... args);

    template<typename... Args>
    void on(const EntityCondition& condition, const Args&... args);

    template<typename... Args>
    void on(const Connector& condition, const Args&... args);

    template<typename... Args>
    void on(const FunctionCondition& condition, const Args&... args);

    template<typename... Args>
    void with(const ConditionConstraint& constaint, const Args&... args);

    template<typename Col, typename... Args>
    void column(const Col& function, const Args&... args);

    template<typename E>
    void columnAll();

    virtual void set() {}
    virtual void filter() {}
    virtual void with() {}
    virtual void column() {}
    virtual void on() {}

    template<typename E>
    void from(Select<E>& select);

    template<typename... E>
    void from(Join<E...>& join);

    void from(RecursiveQueryBuilder& builder);

    void fromDataClear();

    template<typename E>
    void unionSelect(Select<E>& select, bool unionAll = false);

    template<typename... E>
    void unionSelect(Join<E...>& join, bool unionAll = false);

protected:
    bool setThrowable;
    Connector setCondition, columnBind, filterCondition, constraintCondition, onCondition;
    //from sub select
    QString fromSelectStatement;
    QVariantList fromSelectValues;
    QString fromSelectAs;
    bool recursiveQuery;
    //union
    QString unionSelectStatement;
    QVariantList unionSelectValues;
    bool unionAll;

    template<typename E> friend class Insert;
    template<typename E> friend class Select;
    template<typename E> friend class Update;
    template<typename E> friend class Delete;
    template<typename... E> friend class Join;
    template<typename E> friend class InsertIntoSelect;
};

template<typename ...Args>
inline void BaseQueryBuilder::set(const EntityCondition& condition, const Args & ...args) {
    setCondition.append(condition);
    return set(args...);
}

template<typename ...Args>
inline void BaseQueryBuilder::filter(const EntityCondition& condition, const Args & ...args) {
    filterCondition.append(condition);
    filter(args...);
}

template<typename ...Args>
inline void BaseQueryBuilder::filter(const Connector& condition, const Args & ...args) {
    filterCondition.append(condition);
    filter(args...);
}

template<typename ...Args>
inline void BaseQueryBuilder::filter(const FunctionCondition& condition, const Args & ...args) {
    filterCondition.append(condition);
    filter(args...);
}

template<typename ...Args>
inline void BaseQueryBuilder::on(const EntityCondition& condition, const Args & ...args) {
    onCondition.append(condition);
    on(args...);
}

template<typename ...Args>
inline void BaseQueryBuilder::on(const Connector& condition, const Args & ...args) {
    onCondition.append(condition);
    on(args...);
}

template<typename ...Args>
inline void BaseQueryBuilder::on(const FunctionCondition& condition, const Args & ...args) {
    onCondition.append(condition);
    on(args...);
}

template<typename ...Args>
inline void BaseQueryBuilder::with(const ConditionConstraint& constaint, const Args & ...args) {
    constraintCondition.append(constaint);
    with(args...);
}

template<typename Col, typename ...Args>
inline void BaseQueryBuilder::column(const Col& function, const Args & ...args) {
    columnBind.appendCol(function);
    column(args...);
}

template<typename E>
inline void BaseQueryBuilder::columnAll() {
    QString tbName = E::Info::getTableName();
    QStringList fields = E::Info::getFields();
    for (const auto& field : fields) {
        columnBind.appendCol(FieldInfo{ field, tbName });
    }
}

template<typename E>
inline void BaseQueryBuilder::from(Select<E>& select) {
    select.buildFilterSqlStatement();
    fromSelectStatement = select.statement;
    fromSelectValues = select.values;
    if (select.builder->fromSelectAs.isEmpty()) {
        fromSelectAs = "sel_" + E::Info::getTableName();
    } else {
        fromSelectAs = "sel_" + select.builder->fromSelectAs;
    }
}

template<typename ...E>
inline void BaseQueryBuilder::from(Join<E...>& join) {
    join.buildJoinSqlStatement();
    fromSelectStatement = join.statement;
    fromSelectValues = join.values;
    if (join.builder->fromSelectAs.isEmpty()) {
        fromSelectAs = "join_" + join.mainTable;
    } else {
        fromSelectAs = "join_" + join.builder->fromSelectAs;
    }
}

inline void BaseQueryBuilder::from(RecursiveQueryBuilder& builder) {
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

inline void BaseQueryBuilder::fromDataClear() {
    fromSelectStatement.clear();
    fromSelectValues.clear();
    fromSelectAs.clear();
}

template<typename E>
inline void BaseQueryBuilder::unionSelect(Select<E>& select, bool unionAll) {
    select.buildFilterSqlStatement();
    unionSelectStatement = select.statement;
    unionSelectValues = select.values;
    this->unionAll = unionAll;
}

template<typename ...E>
inline void BaseQueryBuilder::unionSelect(Join<E...>& join, bool unionAll) {
    join.buildJoinSqlStatement();
    unionSelectStatement = join.statement;
    unionSelectValues = join.values;
    this->unionAll = unionAll;
}
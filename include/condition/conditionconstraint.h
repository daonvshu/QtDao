#pragma once

#include "entitycondition.h"
#include "entityfield.h"
#include "functioncondition.h"

class Connector;
class ConditionConstraint : public EntityCondition {
public:
    static ConditionConstraint limit(int a, int b);
    static ConditionConstraint limit(int a);

    //order by interface
    template<typename T, typename... E>
    static ConditionConstraint orderBy(const EntityField<T>& field, const EntityField<E>&... n);

    //group by interface
    template<typename T, typename... E>
    static ConditionConstraint groupBy(const EntityField<T>& field, const EntityField<E>&... n);

    static ConditionConstraint having(const EntityCondition& other);

    static ConditionConstraint having(const FunctionCondition& condition);

private:
    //create condition with type order by
    static ConditionConstraint orderBy(const QString& fieldName, const QString& tbName, bool asc = true);
    //create condition with type group by
    static ConditionConstraint groupBy(const QString& fieldName, const QString& tbName);

protected:
    using EntityCondition::EntityCondition;

private:
    EntityCondition havingCondition;
    FunctionCondition havingFunction;

protected:
    void combine() override;
    void combineLimit();
    void combineOrderBy();
    void combineGroupBy();
    void combineHaving();

    QVariantList getValues() override;

    //save field info
    ConditionConstraint& orderBySave(const QString& fieldName, const QString& tbName, bool asc);

    //unpack entities
    template<typename T, typename... E>
    ConditionConstraint& orderByNext(const EntityField<T>& field, const EntityField<E>&... n);

    //end order by unpack entities
    ConditionConstraint& orderByNext() {
        return *this;
    }

    //save field info
    ConditionConstraint& groupBySave(const QString& fieldName, const QString& tbName);

    //unpack entities
    template<typename E, typename... N>
    ConditionConstraint& groupByNext(const EntityField<E>& field, const N&... n);

    //end group by unpack entities
    ConditionConstraint& groupByNext() {
        return *this;
    }

    friend class Connector;
};

template<typename T, typename... E>
inline ConditionConstraint ConditionConstraint::orderBy(const EntityField<T>& field, const EntityField<E>&... n) {
    return orderBy(field.name, field.bindTable).orderByNext(n...);
}

template<typename T, typename... E>
inline ConditionConstraint ConditionConstraint::groupBy(const EntityField<T>& field, const EntityField<E>&... n) {
    return groupBy(field.name, field.bindTable).groupByNext(n...);
}

template<typename T, typename... E>
inline ConditionConstraint& ConditionConstraint::orderByNext(const EntityField<T>& field, const EntityField<E>&... n) {
    return orderBySave(field.name, field.bindTable, true).orderByNext(n...);
}

template<typename E, typename ...N>
inline ConditionConstraint& ConditionConstraint::groupByNext(const EntityField<E>& field, const N & ...n) {
    return groupBySave(field.name, field.bindTable).groupByNext(n...);
}

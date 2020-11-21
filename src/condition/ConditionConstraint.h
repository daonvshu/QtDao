#pragma once

#include "EntityCondition.h"
#include "EntityField.h"

class Connector;
class ConditionConstraint : public EntityCondition {
public:
    static ConditionConstraint limit(int a, int b);
    static ConditionConstraint limit(int a);

    static ConditionConstraint orderBy(const QString& fieldName, bool asc = true);

    template<typename T, typename... E>
    static ConditionConstraint orderBy(const EntityField<T>& field, const EntityField<E>&... n);

    static ConditionConstraint groupBy(const QString& fieldName);

    template<typename T, typename... E>
    static ConditionConstraint groupBy(const EntityField<T>& field, const EntityField<E>&... n);

    //TODO
    template<typename... T>
    static ConditionConstraint having();

protected:
    using EntityCondition::EntityCondition;

protected:
    void combine(const QString& fieldPrefix) override;
    void combineLimit(const QString& fieldPrefix);
    void combineOrderBy(const QString& fieldPrefix);
    void combineGroupBy(const QString& fieldPrefix);

    QVariantList getValues() override;

    ConditionConstraint& orderByNext() {
        return *this;
    }

    ConditionConstraint& orderByNext(const QString& fieldName, bool asc);

    template<typename T, typename... E>
    ConditionConstraint& orderByNext(const EntityField<T>& field, const EntityField<E>&... n);

    ConditionConstraint& groupByNext() {
        return *this;
    }

    ConditionConstraint& groupByNext(const QString& fieldName);

    template<typename E, typename... N>
    ConditionConstraint& groupByNext(const EntityField<E>& field, const N&... n);

    friend class Connector;
};

template<typename T, typename... E>
inline ConditionConstraint ConditionConstraint::orderBy(const EntityField<T>& field, const EntityField<E>&... n) {
    return orderBy(field.name).orderByNext(n...);
}

template<typename T, typename... E>
inline ConditionConstraint ConditionConstraint::groupBy(const EntityField<T>& field, const EntityField<E>&... n) {
    return groupBy(field.name).groupByNext(n...);
}

template<typename T, typename... E>
inline ConditionConstraint& ConditionConstraint::orderByNext(const EntityField<T>& field, const EntityField<E>&... n) {
    return orderByNext(field.name, true).orderByNext(n...);
}

template<typename E, typename ...N>
inline ConditionConstraint& ConditionConstraint::groupByNext(const EntityField<E>& field, const N & ...n) {
    return groupByNext(field.name).groupByNext(n...);
}

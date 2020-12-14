#pragma once

#include <qobject.h>
#include <qvariant.h>
#include <functional>

#include <qsharedpointer.h>

#include "EntityConditionData.h"

template<typename T>
class EntityField;

template<typename E>
class Update;

template<typename E>
class Delete;

class ConditionConstraint;

class Connector;
class EntityCondition {
public:
    EntityCondition() {}

protected:
    //for class EntityField
    EntityCondition(
        const FieldInfo& field,
        const QString& op,
        const QVariant& value,
        bool selfOperate = false
    );

    //for in/between
    EntityCondition(
        const FieldInfo& field,
        const QVariantList& values,
        ConditionType type
    );

    //for constraint
    EntityCondition(
        const QVariantList& values,
        ConditionType type
    );

    //for pair condition
    EntityCondition(
        const QList<FieldInfo>& fields,
        const QString& op
    );

    template<typename T>
    static EntityCondition conditionIn(
        const FieldInfo& field,
        const QList<T>& values
    );

    template<typename T>
    static EntityCondition conditionBetween(
        const FieldInfo& field,
        const T& a, 
        const T& b
    );

protected:
    QSharedDataPointer<EntityConditionData> d;

protected:
    void setFieldPrefixGetter(std::function<QString(const QString&)> prefixGetter);

    virtual void combine();
    void combineNormal();
    void combineIn();
    void combineBetween();

    virtual QVariantList getValues();

    friend class Connector;
    friend class ConditionConstraint;

    template<typename T>
    friend class EntityField;

    template<typename E>
    friend class Update;

    template<typename E>
    friend class Delete;
};

template<typename T>
inline EntityCondition EntityCondition::conditionIn(
    const FieldInfo& field,
    const QList<T>& values
) {
    QVariantList variantValues;
    for (const T& t : values) {
        variantValues << t;
    }
    return EntityCondition(
        field,
        variantValues,
        TypeIn
    );
}

template<typename T>
inline EntityCondition EntityCondition::conditionBetween(
    const FieldInfo& field,
    const T& a, const T& b
) {
    QVariantList values;
    values << a << b;
    return EntityCondition(
        field,
        values,
        TypeBetween
    );
}

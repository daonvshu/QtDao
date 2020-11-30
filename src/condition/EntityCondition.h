#pragma once

#include <qobject.h>
#include <qvariant.h>

#include <qsharedpointer.h>

#include "EntityConditionData.h"

template<typename T>
class EntityField;

template<typename E>
class Update;

template<typename E>
class Delete;

class Connector;
class EntityCondition {
public:
    EntityCondition() {}

protected:
    EntityCondition(
        const QString& fieldName,
        const QString& op,
        const QVariantList& values,
        bool selfOperate = false,
        ConditionType type = TypeNormal
    );

    EntityCondition(
        const QString& fieldName,
        const QVariantList& values,
        ConditionType type
    );

    template<typename T>
    static EntityCondition conditionIn(
        const QString& fieldName,
        const QList<T>& values
    );

    template<typename T>
    static EntityCondition conditionBetween(
        const QString& fieldName,
        const T& a, 
        const T& b
    );

protected:
    QSharedDataPointer<EntityConditionData> d;

protected:
    virtual void combine(const QString& fieldPrefix);
    void combineNormal(const QString& fieldPrefix);
    void combineIn(const QString& fieldPrefix);
    void combineBetween(const QString& fieldPrefix);

    virtual QVariantList getValues();

    friend class Connector;

    template<typename T>
    friend class EntityField;

    template<typename E>
    friend class Update;

    template<typename E>
    friend class Delete;
};

template<typename T>
inline EntityCondition EntityCondition::conditionIn(const QString& fieldName, const QList<T>& values) {
    QVariantList variantValues;
    for (const T& t : values) {
        variantValues << t;
    }
    return EntityCondition(
        fieldName,
        variantValues,
        TypeIn
    );
}

template<typename T>
inline EntityCondition EntityCondition::conditionBetween(const QString& fieldName,  const T& a, const T& b) {
    QVariantList values;
    values << a << b;
    return EntityCondition(
        fieldName,
        values,
        TypeBetween
    );
}

#pragma once

#include <qobject.h>
#include <qvariant.h>

enum ConditionType {
    TypeNormal,
    TypeIn,
    TypeBetween,
    //constraint
    TypeLimit,
    TypeOrderBy,
    TypeGroupBy,
    TypeHaving,
};

template<typename T>
class EntityField;

class Connector;
class EntityCondition {
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
    QString fieldName;
    QString op;
    QVariantList values;
    bool selfOperate;
    QString combineStr;

    ConditionType conditionType;

protected:
    virtual void combine(const QString& fieldPrefix);
    void combineNormal(const QString& fieldPrefix);
    void combineIn(const QString& fieldPrefix);
    void combineBetween(const QString& fieldPrefix);

    virtual QVariantList getValues() {
        return values;
    }

    friend class Connector;

    template<typename T>
    friend class EntityField;
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

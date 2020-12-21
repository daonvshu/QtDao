#pragma once

#include <qobject.h>
#include <qvariant.h>

#include "EntityCondition.h"

class ConditionConstraint;
class FunctionCondition;
class Connector;

template<typename T>
class EntityField {
public:
    explicit EntityField(const QString& fieldName, const QString& bindTable)
        : name(fieldName)
        , bindTable(bindTable)
    {}

    /*get name*/
    QString operator()() const {
        return name;
    }

private:
    QString name;
    QString bindTable;

    friend class ConditionConstraint;
    friend class FunctionCondition;
    friend class Connector;

private:
    EntityCondition setValue(const char* op, const T& v) {
        return EntityCondition(FieldInfo{ name, bindTable }, op, v);
    }

    EntityCondition setValueSelf(const char* op, const T& v) {
        return EntityCondition(FieldInfo{ name, bindTable }, op, v, true);
    }

    EntityCondition setValue(const char* op, const QList<T>& v) {
        QVariantList values;
        for (const auto& i : v) {
            values << i;
        }
        return EntityCondition(FieldInfo{ name, bindTable }, op, values);
    }

    EntityCondition setValueSelf(const char* op, const QList<T>& v) {
        QVariantList values;
        for (const auto& i : v) {
            values << i;
        }
        return EntityCondition(FieldInfo{ name, bindTable }, op, values, true);
    }

    EntityCondition setValue(const char* op, const EntityField<T>& other) {
        QList<FieldInfo> info;
        info << FieldInfo{ name, bindTable };
        info << FieldInfo{ other.name, other.bindTable };
        return EntityCondition(info, op);
    }

public:
    /*equal "="*/
    EntityCondition operator==(const T& v) {
        return setValue("=", v);
    }
    EntityCondition operator==(const QList<T>& v) {
        return setValue("=", v);
    }
    EntityCondition operator==(const EntityField<T>& other) {
        return setValue("=", other);
    }
    /*no equal "!="*/
    EntityCondition operator!=(const T& v) {
        return setValue("!=", v);
    }
    EntityCondition operator!=(const QList<T>& v) {
        return setValue("!=", v);
    }
    EntityCondition operator!=(const EntityField<T>& other) {
        return setValue("!=", other);
    }
    /*greater than ">"*/
    EntityCondition operator>(const T& v) {
        return setValue(">", v);
    }
    EntityCondition operator>(const QList<T>& v) {
        return setValue(">", v);
    }
    EntityCondition operator>(const EntityField<T>& other) {
        return setValue(">", other);
    }
    /*greater than and equal ">=" */
    EntityCondition operator>=(const T& v) {
        return setValue(">=", v);
    }
    EntityCondition operator>=(const QList<T>& v) {
        return setValue(">=", v);
    }
    EntityCondition operator>=(const EntityField<T>& other) {
        return setValue(">=", other);
    }
    /*less than "<"*/
    EntityCondition operator<(const T& v) {
        return setValue("<", v);
    }
    EntityCondition operator<(const QList<T>& v) {
        return setValue("<", v);
    }
    EntityCondition operator<(const EntityField<T>& other) {
        return setValue("<", other);
    }
    /*less than and equal "<="*/
    EntityCondition operator<=(const T& v) {
        return setValue("<=", v);
    }
    EntityCondition operator<=(const QList<T>& v) {
        return setValue("<=", v);
    }
    EntityCondition operator<=(const EntityField<T>& other) {
        return setValue("<=", other);
    }

    /*like "like"*/
    EntityCondition like(const T& v) {
        return setValue(" like ", v);
    }
    EntityCondition like(const QList<T>& v) {
        return setValue(" like ", v);
    }

    /*like sqlite "glob"*/
    EntityCondition glob(const T& v) {
        return setValue(" glob ", v);
    }
    EntityCondition glob(const QList<T>& v) {
        return setValue(" glob ", v);
    }

    //set condition
    EntityCondition operator=(const T& v) {
        return setValue("=", v);
    }
    EntityCondition operator=(const QList<T>& v) {
        return setValue("=", v);
    }
    /*mod "a=a%value*/
    EntityCondition operator%(const T& v) {
        return setValueSelf("%", v);
    }
    EntityCondition operator%(const QList<T>& v) {
        return setValueSelf("%", v);
    }
    /*plus value "a=a+value"*/
    EntityCondition operator+(const T& v) {
        return setValueSelf("+", v);
    }
    EntityCondition operator+(const QList<T>& v) {
        return setValueSelf("+", v);
    }
    /*minus value "a=a-value"*/
    EntityCondition operator-(const T& v) {
        return setValueSelf("-", v);
    }
    EntityCondition operator-(const QList<T>& v) {
        return setValueSelf("-", v);
    }
    /*repeat count "a=a*count"*/
    EntityCondition operator*(const T& v) {
        return setValueSelf("*", v);
    }
    EntityCondition operator*(const QList<T>& v) {
        return setValueSelf("*", v);
    }
    /*divide count "a=a/count"*/
    EntityCondition operator/(const T& v) {
        return setValueSelf("/", v);
    }
    EntityCondition operator/(const QList<T>& v) {
        return setValueSelf("/", v);
    }

    /*plus one "a=a+1"*/
    EntityCondition operator++(int) {
        return setValueSelf("+", 1);
    }
    /*minus one "a=a-1"*/
    EntityCondition operator--(int) {
        return setValueSelf("-", 1);
    }

    /*condition in*/
    EntityCondition in(const QList<T>& value) {
        return EntityCondition::conditionIn(FieldInfo{name, bindTable}, value);
    }

    /*condition between*/
    EntityCondition between(const T& a, const T& b) {
        return EntityCondition::conditionBetween(FieldInfo{ name, bindTable }, a, b);
    }

    /*condition is null*/
    EntityCondition lsNull() {
        return EntityCondition::conditionIs(FieldInfo{ name, bindTable }, false);
    }

    /*condition is not null*/
    EntityCondition notNull() {
        return EntityCondition::conditionIs(FieldInfo{ name, bindTable }, true);
    }

    /*for order by*/
    EntityField desc() {
        return EntityField(name + " desc", bindTable);
    }

    /*distinct*/
    EntityField distinct() {
        return EntityField("distinct " + name, bindTable);
    }
};


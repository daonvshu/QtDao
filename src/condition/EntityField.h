#pragma once

#include <qobject.h>
#include <qvariant.h>

#include "EntityCondition.h"

class ConditionConstraint;

template<typename T>
class EntityField {
public:
    EntityField(const char* fieldName) : name(fieldName) {}

private:
    QString name;

private:
    EntityField(const QString& name) : name(name) {}

    EntityCondition setValue(const char* op, const T& v) {
        return EntityCondition(name, op, QVariantList() << v);
    }

    EntityCondition setValueSelf(const char* op, const T& v) {
        return EntityCondition(name, op, QVariantList() << v, true);
    }

public:
    /*equal "="*/
    EntityCondition operator==(const T& v) {
        return setValue("=", v);
    }
    /*no equal "!="*/
    EntityCondition operator!=(const T& v) {
        return setValue("!=", v);
    }
    /*greater than ">"*/
    EntityCondition operator>(const T& v) {
        return setValue(">", v);
    }
    /*greater than and equal ">=" */
    EntityCondition operator>=(const T& v) {
        return setValue(">=", v);
    }
    /*less than "<"*/
    EntityCondition operator<(const T& v) {
        return setValue("<", v);
    }
    /*less than and equal "<="*/
    EntityCondition operator<=(const T& v) {
        return setValue("<=", v);
    }

    /*like "like"*/
    EntityCondition like(const T& v) {
        return setValue(" like ", v);
    }

    //set condition
    /*mod "a=a%value*/
    EntityCondition operator%(const T& v) {
        return setValueSelf("%", v);
    }
    /*plus value "a=a+value"*/
    EntityCondition operator+(const T& v) {
        return setValueSelf("+", v);
    }
    /*minus value "a=a-value"*/
    EntityCondition operator-(const T& v) {
        return setValueSelf("-", v);
    }
    /*repeat count "a=a*count"*/
    EntityCondition operator*(const T& v) {
        return setValueSelf("*", v);
    }
    /*divide count "a=a/count"*/
    EntityCondition operator/(const T& v) {
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
        return EntityCondition::conditionIn(name, value);
    }

    /*condition between*/
    EntityCondition between(const T& a, const T& b) {
        return EntityCondition::conditionBetween(name, a, b);
    }

    /*for order by*/
    EntityField desc() {
        return EntityField(name + " desc");
    }

    friend class EntityCondition;
    friend class ConditionConstraint;
};


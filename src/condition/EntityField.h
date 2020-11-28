#pragma once

#include <qobject.h>
#include <qvariant.h>

#include "EntityCondition.h"

template<typename T>
class EntityField {
public:
    explicit EntityField(const char* fieldName) : name(fieldName) {}

    /*get name*/
    QString operator()() const {
        return name;
    }

private:
    QString name;

private:
    EntityCondition setValue(const char* op, const T& v) {
        return EntityCondition(name, op, QVariantList() << v);
    }

    EntityCondition setValueSelf(const char* op, const T& v) {
        return EntityCondition(name, op, QVariantList() << v, true);
    }

    EntityCondition setValue(const char* op, const QList<T>& v) {
        QVariantList values;
        for (const auto& i : v) {
            values << i;
        }
        return EntityCondition(name, op, QVariantList() << QVariant(values));
    }

    EntityCondition setValueSelf(const char* op, const QList<T>& v) {
        QVariantList values;
        for (const auto& i : v) {
            values << i;
        }
        return EntityCondition(name, op, QVariantList() << QVariant(values), true);
    }

public:
    /*equal "="*/
    EntityCondition operator==(const T& v) {
        return setValue("=", v);
    }
    EntityCondition operator==(const QList<T>& v) {
        return setValue("=", v);
    }
    /*no equal "!="*/
    EntityCondition operator!=(const T& v) {
        return setValue("!=", v);
    }
    EntityCondition operator!=(const QList<T>& v) {
        return setValue("!=", v);
    }
    /*greater than ">"*/
    EntityCondition operator>(const T& v) {
        return setValue(">", v);
    }
    EntityCondition operator>(const QList<T>& v) {
        return setValue(">", v);
    }
    /*greater than and equal ">=" */
    EntityCondition operator>=(const T& v) {
        return setValue(">=", v);
    }
    EntityCondition operator>=(const QList<T>& v) {
        return setValue(">=", v);
    }
    /*less than "<"*/
    EntityCondition operator<(const T& v) {
        return setValue("<", v);
    }
    EntityCondition operator<(const QList<T>& v) {
        return setValue("<", v);
    }
    /*less than and equal "<="*/
    EntityCondition operator<=(const T& v) {
        return setValue("<=", v);
    }
    EntityCondition operator<=(const QList<T>& v) {
        return setValue("<=", v);
    }

    /*like "like"*/
    EntityCondition like(const T& v) {
        return setValue(" like ", v);
    }
    EntityCondition like(const QList<T>& v) {
        return setValue(" like ", v);
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
        return EntityCondition::conditionIn(name, value);
    }

    /*condition between*/
    EntityCondition between(const T& a, const T& b) {
        return EntityCondition::conditionBetween(name, a, b);
    }

    /*for order by*/
    EntityField desc() {
        return EntityField((name + " desc").toUtf8().constData());
    }
};


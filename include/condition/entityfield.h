#pragma once

#include "../global.h"

#include <qobject.h>
#include <qvariant.h>

#include <utility>

#include "entitycondition.h"

QTDAO_BEGIN_NAMESPACE

class ConditionConstraint;
class FunctionCondition;
class Connector;

template<typename T>
class EntityField {
public:
    explicit EntityField(QString fieldName, QString bindTable, bool isCustomType = false)
        : name(std::move(fieldName))
        , bindTable(std::move(bindTable))
        , customType(isCustomType)
    {}

    /*get name*/
    QString operator()() const {
        return name;
    }

private:
    QString name;
    QString bindTable;
    bool customType;

    friend class ConditionConstraint;
    friend class FunctionCondition;
    friend class Connector;

private:
    Connectable* setValue(const char* op, const T& v) {
        auto connector = new OperatorEntityConnector;
        connector->setOperator(op);
        connector->addField(FieldInfo{ name, bindTable });
        if (customType) {
            connector->addCustomValue(v);
        } else {
            connector->addValue(v);
        }
        return connector;
    }

    Connectable* setValueSelf(const char* op, const T& v) {
        auto connector = new SelfOperatorEntityConnector;
        connector->setOperator(op);
        connector->addField(FieldInfo{ name, bindTable });
        if (customType) {
            connector->addCustomValue(v);
        } else {
            connector->addValue(v);
        }
        return connector;
    }

    Connectable* setValues(const char* op, const QList<T>& v) {
        auto connector = new BatchOperatorEntityConnector<T>;
        connector->setOperator(op);
        connector->addField(FieldInfo{ name, bindTable });
        if (customType) {
            connector->addCustomValues(v);
        } else {
            connector->addValues(v);
        }
        return connector;
    }

    Connectable* setValuesSelf(const char* op, const QList<T>& v) {
        auto connector = new BatchSelfOperatorEntityConnector<T>;
        connector->setOperator(op);
        connector->addField(FieldInfo{ name, bindTable });
        if (customType) {
            connector->addCustomValues(v);
        } else {
            connector->addValues(v);
        }
        return connector;
    }

    Connectable* setFieldValue(const char* op, const EntityField<T>& other) {
        auto connector = new FieldOperatorEntityConnector;
        connector->addFields(FieldInfo{ name, bindTable }, FieldInfo{ other.name, other.bindTable });
        connector->setOperator(op);
        return connector;
    }

    Connectable* setFieldValueSelf(const char* op, const EntityField<T>& other) {
        auto connector = new SelfFieldOperatorEntityConnector;
        connector->addFields(FieldInfo{ name, bindTable }, FieldInfo{ other.name, other.bindTable });
        connector->setOperator(op);
        return connector;
    }

public:
    /*equal "="*/
    Connectable* operator==(const T& v) {
        return setValue("=", v);
    }
    Connectable* operator==(const QList<T>& v) {
        return setValues("=", v);
    }
    Connectable* operator==(const EntityField<T>& other) {
        return setFieldValue("=", other);
    }
    /*no equal "!="*/
    Connectable* operator!=(const T& v) {
        return setValue("!=", v);
    }
    Connectable* operator!=(const QList<T>& v) {
        return setValues("!=", v);
    }
    Connectable* operator!=(const EntityField<T>& other) {
        return setFieldValue("!=", other);
    }
    /*greater than ">"*/
    Connectable* operator>(const T& v) {
        return setValue(">", v);
    }
    Connectable* operator>(const QList<T>& v) {
        return setValues(">", v);
    }
    Connectable* operator>(const EntityField<T>& other) {
        return setFieldValue(">", other);
    }
    /*greater than and equal ">=" */
    Connectable* operator>=(const T& v) {
        return setValue(">=", v);
    }
    Connectable* operator>=(const QList<T>& v) {
        return setValues(">=", v);
    }
    Connectable* operator>=(const EntityField<T>& other) {
        return setFieldValue(">=", other);
    }
    /*less than "<"*/
    Connectable* operator<(const T& v) {
        return setValue("<", v);
    }
    Connectable* operator<(const QList<T>& v) {
        return setValues("<", v);
    }
    Connectable* operator<(const EntityField<T>& other) {
        return setFieldValue("<", other);
    }
    /*less than and equal "<="*/
    Connectable* operator<=(const T& v) {
        return setValue("<=", v);
    }
    Connectable* operator<=(const QList<T>& v) {
        return setValues("<=", v);
    }
    Connectable* operator<=(const EntityField<T>& other) {
        return setFieldValue("<=", other);
    }

    /*like "like"*/
    Connectable* like(const T& v) {
        return setValue(" like ", v);
    }
    Connectable* like(const QList<T>& v) {
        return setValues(" like ", v);
    }
    Connectable* like(const EntityField<T>& other) {
        return setFieldValue(" like ", other);
    }

    /*like sqlite "glob"*/
    Connectable* glob(const T& v) {
        return setValue(" glob ", v);
    }
    Connectable* glob(const QList<T>& v) {
        return setValues(" glob ", v);
    }
    Connectable* glob(const EntityField<T>& other) {
        return setFieldValue(" glob ", other);
    }

    //set condition
    Connectable* operator=(const T& v) {
        return setValue("=", v);
    }
    Connectable* operator=(const QList<T>& v) {
        return setValues("=", v);
    }
    Connectable* operator=(const EntityField<T>& other) {
        return setFieldValue("=", other);
    }
    /*mod "a=a%value*/
    Connectable* operator%(const T& v) {
        return setValueSelf("%", v);
    }
    Connectable* operator%(const QList<T>& v) {
        return setValuesSelf("%", v);
    }
    Connectable* operator%(const EntityField<T>& other) {
        return setFieldValueSelf("%", other);
    }
    /*plus value "a=a+value"*/
    Connectable* operator+(const T& v) {
        return setValueSelf("+", v);
    }
    Connectable* operator+(const QList<T>& v) {
        return setValuesSelf("+", v);
    }
    Connectable* operator+(const EntityField<T>& other) {
        return setFieldValueSelf("+", other);
    }
    /*minus value "a=a-value"*/
    Connectable* operator-(const T& v) {
        return setValueSelf("-", v);
    }
    Connectable* operator-(const QList<T>& v) {
        return setValuesSelf("-", v);
    }
    Connectable* operator-(const EntityField<T>& other) {
        return setFieldValueSelf("-", other);
    }
    /*repeat count "a=a*count"*/
    Connectable* operator*(const T& v) {
        return setValueSelf("*", v);
    }
    Connectable* operator*(const QList<T>& v) {
        return setValuesSelf("*", v);
    }
    Connectable* operator*(const EntityField<T>& other) {
        return setFieldValueSelf("*", other);
    }
    /*divide count "a=a/count"*/
    Connectable* operator/(const T& v) {
        return setValueSelf("/", v);
    }
    Connectable* operator/(const QList<T>& v) {
        return setValuesSelf("/", v);
    }
    Connectable* operator/(const EntityField<T>& other) {
        return setFieldValueSelf("/", other);
    }

    /*plus one "a=a+1"*/
    Connectable* operator++(int) {
        return setValueSelf("+", 1);
    }
    /*minus one "a=a-1"*/
    Connectable* operator--(int) {
        return setValueSelf("-", 1);
    }

    /*condition in*/
    Connectable* in(const QList<T>& value) {
        auto connector = new InEntityConnector;
        connector->addField(FieldInfo{name, bindTable});
        if (customType) {
            connector->addCustomValues(value);
        } else {
            connector->addValues(value);
        }
        return connector;
    }

    /*condition between*/
    Connectable* between(const T& a, const T& b) {
        auto connector = new BetweenConnector;
        connector->addField(FieldInfo{name, bindTable});
        connector->addValues(a, b);
        return connector;
    }

    /*condition is null*/
    Connectable* lsNull() {
        auto connector = new IsNullConnector(true);
        connector->addField(FieldInfo{name, bindTable});
        return connector;
    }

    /*condition is not null*/
    Connectable* notNull() {
        auto connector = new IsNullConnector(false);
        connector->addField(FieldInfo{name, bindTable});
        return connector;
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

QTDAO_END_NAMESPACE
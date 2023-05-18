#pragma once

#include "../global.h"

#include <qobject.h>
#include <qvariant.h>

#include <utility>

#include "entitycondition.h"

QTDAO_BEGIN_NAMESPACE

template<typename T>
class EntityField {
public:
    EntityField(QString fieldName, QString bindTable, bool isCustomType = false)
        : name(std::move(fieldName))
        , bindTable(std::move(bindTable))
        , customType(isCustomType)
    {}

    /*get name*/
    QString operator()() const {
        return name;
    }

    void resetTb(const QString& tbName) {
        this->bindTable = tbName;
    }

    using Type = T;

private:
    QString name;
    QString bindTable;
    bool customType;

private:
    FieldInfo getFieldInfo() const {
        return FieldInfo{ name, bindTable };
    }

    friend class Connectable;
    friend class JoinImpl;

private:
    template<typename E>
    QVariant getSupportedValue(const typename std::enable_if<std::is_constructible<QVariant, E>::value, E>::type& v) {
        return v;
    }

    template<typename E>
    QVariant getSupportedValue(const typename std::enable_if<!std::is_constructible<QVariant, E>::value, E>::type& v) {
        qFatal("used error type!");
        return {};
    }

    template<typename E>
    QVariant getSupportedValue(const typename std::enable_if<std::is_constructible<QVariant, E>::value, QList<E>>::type& v) {
        return v;
    }

    template<typename E>
    QVariant getSupportedValue(const typename std::enable_if<!std::is_constructible<QVariant, E>::value, QList<E>>::type& v) {
        qFatal("used error type!");
        return {};
    }

    template<typename E>
    const QList<E>& getSupportedValues(const typename std::enable_if<std::is_constructible<QVariant, E>::value, QList<E>>::type& v) {
        return v;
    }

    template<typename E>
    QList<E> getSupportedValues(const typename std::enable_if<!std::is_constructible<QVariant, E>::value, QList<E>>::type& v) {
        qFatal("used error type!");
        return {};
    }

    template<typename EC>
    void setConnectorValue(EC* connector, const T& v) {
        connector->addValue(getSupportedValue<T>(v));
    }

    template<typename EC>
    void setConnectorValue(EC* connector, const QList<T>& v) {
        connector->addValue(getSupportedValue<T>(v));
    }

    template<typename EC>
    void setConnectorBatchValue(EC* connector, const QList<T>& v) {
        connector->addValues(getSupportedValues<T>(v));
    }

    EntityConnector* setValue(const char* op, const T& v) {
        auto connector = new OperatorEntityConnector;
        connector->setOperator(op);
        connector->addField(getFieldInfo());
        if (customType) {
            connector->addCustomValue(v);
        } else {
            setConnectorValue(connector, v);
        }
        return connector->ptr();
    }

    EntityConnector* setValueSelf(const char* op, const T& v) {
        auto connector = new SelfOperatorEntityConnector;
        connector->setOperator(op);
        connector->addField(getFieldInfo());
        if (customType) {
            connector->addCustomValue(v);
        } else {
            setConnectorValue(connector, v);
        }
        return connector->ptr();
    }

    EntityConnector* setValues(const char* op, const QList<T>& v) {
        auto connector = new BatchOperatorEntityConnector<T>;
        connector->setOperator(op);
        connector->addField(getFieldInfo());
        if (customType) {
            connector->addCustomValues(v);
        } else {
            setConnectorBatchValue(connector, v);
        }
        return connector->ptr();
    }

    EntityConnector* setValuesSelf(const char* op, const QList<T>& v) {
        auto connector = new BatchSelfOperatorEntityConnector<T>;
        connector->setOperator(op);
        connector->addField(getFieldInfo());
        if (customType) {
            connector->addCustomValues(v);
        } else {
            setConnectorBatchValue(connector, v);
        }
        return connector->ptr();
    }

    EntityConnector* setFieldValue(const char* op, const EntityField<T>& other) {
        auto connector = new FieldOperatorEntityConnector;
        connector->addFields(getFieldInfo(), other.getFieldInfo());
        connector->setOperator(op);
        return connector->ptr();
    }

    EntityConnector* setFieldValueSelf(const char* op, const EntityField<T>& other) {
        auto connector = new SelfFieldOperatorEntityConnector;
        connector->addFields(getFieldInfo(), other.getFieldInfo());
        connector->setOperator(op);
        return connector->ptr();
    }

public:
    /*equal "="*/
    EntityConnector* operator==(const T& v) {
        return setValue("=", v);
    }
    EntityConnector* operator==(const QList<T>& v) {
        return setValues("=", v);
    }
    EntityConnector* operator==(const EntityField<T>& other) {
        return setFieldValue("=", other);
    }
    /*no equal "!="*/
    EntityConnector* operator!=(const T& v) {
        return setValue("!=", v);
    }
    EntityConnector* operator!=(const QList<T>& v) {
        return setValues("!=", v);
    }
    EntityConnector* operator!=(const EntityField<T>& other) {
        return setFieldValue("!=", other);
    }
    /*greater than ">"*/
    EntityConnector* operator>(const T& v) {
        return setValue(">", v);
    }
    EntityConnector* operator>(const QList<T>& v) {
        return setValues(">", v);
    }
    EntityConnector* operator>(const EntityField<T>& other) {
        return setFieldValue(">", other);
    }
    /*greater than and equal ">=" */
    EntityConnector* operator>=(const T& v) {
        return setValue(">=", v);
    }
    EntityConnector* operator>=(const QList<T>& v) {
        return setValues(">=", v);
    }
    EntityConnector* operator>=(const EntityField<T>& other) {
        return setFieldValue(">=", other);
    }
    /*less than "<"*/
    EntityConnector* operator<(const T& v) {
        return setValue("<", v);
    }
    EntityConnector* operator<(const QList<T>& v) {
        return setValues("<", v);
    }
    EntityConnector* operator<(const EntityField<T>& other) {
        return setFieldValue("<", other);
    }
    /*less than and equal "<="*/
    EntityConnector* operator<=(const T& v) {
        return setValue("<=", v);
    }
    EntityConnector* operator<=(const QList<T>& v) {
        return setValues("<=", v);
    }
    EntityConnector* operator<=(const EntityField<T>& other) {
        return setFieldValue("<=", other);
    }

    /*like "like"*/
    EntityConnector* like(const T& v) {
        return setValue(" like ", v);
    }
    EntityConnector* like(const QList<T>& v) {
        return setValues(" like ", v);
    }
    EntityConnector* like(const EntityField<T>& other) {
        return setFieldValue(" like ", other);
    }

    /*like sqlite "glob"*/
    EntityConnector* glob(const T& v) {
        return setValue(" glob ", v);
    }
    EntityConnector* glob(const QList<T>& v) {
        return setValues(" glob ", v);
    }
    EntityConnector* glob(const EntityField<T>& other) {
        return setFieldValue(" glob ", other);
    }

    //set condition
    EntityConnector* operator=(const T& v) {
        return setValue("=", v);
    }
    EntityConnector* operator=(const QList<T>& v) {
        return setValues("=", v);
    }
    EntityConnector* operator=(const EntityField<T>& other) {
        return setFieldValue("=", other);
    }
    /*mod "a=a%value*/
    EntityConnector* operator%(const T& v) {
        return setValueSelf("%", v);
    }
    EntityConnector* operator%(const QList<T>& v) {
        return setValuesSelf("%", v);
    }
    EntityConnector* operator%(const EntityField<T>& other) {
        return setFieldValueSelf("%", other);
    }
    /*plus value "a=a+value"*/
    EntityConnector* operator+(const T& v) {
        return setValueSelf("+", v);
    }
    EntityConnector* operator+(const QList<T>& v) {
        return setValuesSelf("+", v);
    }
    EntityConnector* operator+(const EntityField<T>& other) {
        return setFieldValueSelf("+", other);
    }
    /*minus value "a=a-value"*/
    EntityConnector* operator-(const T& v) {
        return setValueSelf("-", v);
    }
    EntityConnector* operator-(const QList<T>& v) {
        return setValuesSelf("-", v);
    }
    EntityConnector* operator-(const EntityField<T>& other) {
        return setFieldValueSelf("-", other);
    }
    /*repeat count "a=a*count"*/
    EntityConnector* operator*(const T& v) {
        return setValueSelf("*", v);
    }
    EntityConnector* operator*(const QList<T>& v) {
        return setValuesSelf("*", v);
    }
    EntityConnector* operator*(const EntityField<T>& other) {
        return setFieldValueSelf("*", other);
    }
    /*divide count "a=a/count"*/
    EntityConnector* operator/(const T& v) {
        return setValueSelf("/", v);
    }
    EntityConnector* operator/(const QList<T>& v) {
        return setValuesSelf("/", v);
    }
    EntityConnector* operator/(const EntityField<T>& other) {
        return setFieldValueSelf("/", other);
    }

    /*plus one "a=a+1"*/
    EntityConnector* operator++(int) {
        return setValueSelf("+", 1);
    }
    /*minus one "a=a-1"*/
    EntityConnector* operator--(int) {
        return setValueSelf("-", 1);
    }

    /*condition in*/
    EntityConnector* in(const QList<T>& value) {
        auto connector = new InEntityConnector;
        connector->addField(getFieldInfo());
        if (customType) {
            connector->addCustomValues(value);
        } else {
            setConnectorBatchValue(connector, value);
        }
        return connector->ptr();
    }

    /*condition between*/
    EntityConnector* between(const T& a, const T& b) {
        auto connector = new BetweenConnector;
        connector->addField(getFieldInfo());
        connector->addValues(a, b);
        return connector->ptr();
    }

    /*condition is null*/
    EntityConnector* lsNull() {
        auto connector = new IsNullConnector(true);
        connector->addField(getFieldInfo());
        return connector->ptr();
    }

    /*condition is not null*/
    EntityConnector* notNull() {
        auto connector = new IsNullConnector(false);
        connector->addField(getFieldInfo());
        return connector->ptr();
    }

    /*for order by*/
    EntityField desc() {
        return EntityField(name + " desc", bindTable, customType);
    }

    /*distinct*/
    EntityField distinct() {
        return EntityField("distinct " + name, bindTable, customType);
    }
};

template<typename T>
inline FieldInfo Connectable::getEntityFieldInfo(const EntityField<T>& entityField) {
    return entityField.getFieldInfo();
}

QTDAO_END_NAMESPACE
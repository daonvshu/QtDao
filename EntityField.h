#pragma once

#include <qobject.h>
#include <qvariant.h>
#include "EntityConditionInterface.h"
#include "EntityConditions.h"
#include "EntityFunction.h"

class EntityField : public EntityConditionInterface {

public:
    EntityField(const char* field) : asc_value(true), bindOrderIndex(0), is_funtion(false) {
        this->field = field;
    }

    EntityField(const QString& fieldStr) : asc_value(true), bindOrderIndex(0), is_funtion(false) {
        this->field = field;
    }

    static void bindTableOrder(int o) {
    }

    /*绑定join前缀*/
    template<typename... T>
    static void bindTableOrder(int o, EntityField& f, T&... t) {
        f.joinPrefix = ('a' + o);
        f.joinPrefix.append('.');
        f.bindOrderIndex = o;
        bindTableOrder(o, t...);
    }

    static void clearTableOrder() {
    }

    /*清除join前缀*/
    template<typename... T>
    static void clearTableOrder(EntityField& f, T&... t) {
        f.joinPrefix = "";
        f.bindOrderIndex = 0;
        clearTableOrder(t...);
    }

    QString getExpressionStr(bool withoutCombineOp = false) const override;
    QVariantList getValueList() const override;
    bool isAsc() const override;

    EntityConditions createCondtionWithoutKv() const;

    QString operator()() const {
        return fieldWithJoinPrefix();
    }

    inline int getBindOrderIndex() const {
        return bindOrderIndex;
    }

    QString fieldWithJoinPrefix() const;//没有使用join时不会有前缀
    QString fieldWithoutJoinPredix() const;//返回单纯的字段名称，join bind字段时使用

    inline bool isFuntion() const {
        return is_funtion;
    }

private:
    QString kvPairStr;//键值对字符串
    QVariantList valueList;//值

    QString field;//字段名称
    QString joinPrefix;//join前缀
    int bindOrderIndex;

    QString conditionCombineOp;//条件连接符，单独使用，用于占位

    bool asc_value;//配合order by使用

    bool is_funtion;//区分字段和funtion类型
    QString asField;//as别名

private:
    EntityField bindValue(const QString& kvPairStr, const QVariantList& value);

    EntityConditions setValue(const char* op, const QVariant& v);
    EntityConditions setValue(const char* op, const EntityConditionInterface& v);

    EntityConditions setValueSelf(const char* op, const QVariant& v);
    EntityConditions setValueSelf(const char* op, const EntityConditionInterface& v);

    static EntityField createCombineOp(const char* op);
    bool isCombineOpType() const;

    friend class EntityConditions;
    friend class EntityFunction;

public:
    //where condition
    /*equal "="*/
    template<typename T>
    EntityConditions operator==(const T& v) {
        return setValue("=", v);
    }
    /*no equal "!="*/
    template<typename T>
    EntityConditions operator!=(const T& v) {
        return setValue("!=", v);
    }
    /*greater than ">"*/
    template<typename T>
    EntityConditions operator>(const T& v) {
        return setValue(">", v);
    }
    /*greater than and equal ">=" */
    template<typename T>
    EntityConditions operator>=(const T& v) {
        return setValue(">=", v);
    }
    /*less than "<"*/
    template<typename T>
    EntityConditions operator<(const T& v) {
        return setValue("<", v);
    }
    /*less than and equal "<="*/
    template<typename T>
    EntityConditions operator<=(const T& v) {
        return setValue("<=", v);
    }

    /*like "like"*/
    template<typename T>
    EntityConditions like(const T& v) {
        return setValue(" like ", v);
    }

    //set condition
    /*mod "a=a%value*/
    template<typename T>
    EntityConditions operator%(const T& v) {
        return setValueSelf("%", v);
    }
    /*plus value "a=a+value"*/
    template<typename T>
    EntityConditions operator+(const T& v) {
        return setValueSelf("+", v);
    }
    /*minus value "a=a-value"*/
    template<typename T>
    EntityConditions operator-(const T& v) {
        return setValueSelf("-", v);
    }
    /*repeat count "a=a*count"*/
    template<typename T>
    EntityConditions operator*(const T& v) {
        return setValueSelf("*", v);
    }
    /*divide count "a=a/count"*/
    template<typename T>
    EntityConditions operator/(const T& v) {
        return setValueSelf("/", v);
    }

    /*plus one "a=a+1"*/
    EntityConditions operator++(int) {
        return setValueSelf("+", 1);
    }
    /*minus one "a=a-1"*/
    EntityConditions operator--(int) {
        return setValueSelf("-", 1);
    }

    /*condition in*/
    EntityConditions in(const QVariantList& v);
    /*condition in(expression)*/
    EntityConditions in(const EntityFunction& v);

    /*condition between*/
    EntityConditions between(const QVariant& a, const QVariant& b);
    /*condition between*/
    EntityConditions between(const EntityField& a, const EntityField& b);
    /*condition between */

    /*配合order by使用，默认asc*/
    EntityField asc();
    /*配合order by使用*/
    EntityField desc();

    /*用在bind函数中*/
    EntityConditions operator,(const EntityField& oth);
};

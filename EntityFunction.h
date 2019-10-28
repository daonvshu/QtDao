#pragma once

#include <qobject.h>
#include <qvariant.h>
#include "EntityConditionInterface.h"

class EntityConditions;
class EntityField;
class EntityFunction : public EntityConditionInterface {

public:
    EntityFunction() : asc_value(true) {
        detach_num = 0;
    }

    //sub condition
    /*limit a,b*/
    EntityFunction limit(int a, int b);
    /*equal call limit(0,a)*/
    EntityFunction limit(int a);

    /*order by field asc/desc*/
    EntityFunction orderBy(const EntityConditionInterface& field);
    /*order by field asc/desc*/
    template<typename... Args>
    EntityFunction orderBy(const EntityConditionInterface& t, const Args&... args) {
        return orderBy(t).orderBy(args...);
    }

    /*group by field*/
    EntityFunction groupBy(const EntityConditionInterface& field);
    /*group by field*/
    template<typename... Args>
    EntityFunction groupBy(const EntityConditionInterface& t, const Args&... args) {
        return groupBy(t).groupBy(args...);
    }

    /*having condition*/
    EntityFunction having(const EntityConditionInterface& conditions);

    /*为condition添加一个小括号 "(condition)"*/
    EntityConditions& operator()(EntityConditions& oth);

    EntityConditions operator&&(const EntityConditions& oth);
    EntityConditions operator&&(const EntityFunction& oth);

    EntityConditions operator||(const EntityConditions& oth);
    EntityConditions operator||(const EntityFunction& oth);

    EntityConditions operator,(const EntityConditions& oth);
    EntityConditions operator,(const EntityFunction& oth);

    /*function expression string*/
    EntityFunction& expStr(const char* str);
    /*add field list*/
    EntityFunction& field(const EntityField& field);
    /*add field list*/
    template<typename... Args>
    EntityFunction& field(const EntityField& fd, const Args&... args) {
        return field(fd).field(args...);
    }

    /*bind values*/
    EntityFunction& value(QVariant v);
    /*bind values*/
    template<typename... Args>
    EntityFunction& value(QVariant v, Args... args) {
        return value(v).value(args...);
    }

    /*create new function*/
    EntityFunction fun() {
        return EntityFunction();
    }

    QString getExpressionStr(bool withoutCombineOp = false) const override;
    QVariantList getValueList() const override;
    bool isAsc() const override;

private:
    QString expressionStr;//表达式字符串
    QVariantList valueList;//值
    int detach_num;
    bool asc_value;
    QString as_field;//as别名

    friend class EntityField;
    friend class EntityConditions;

private:
    EntityConditions createCondition() const;

public:
    /*配合order by使用，默认asc*/
    EntityFunction asc();
    /*配合order by使用*/
    EntityFunction desc();
    /*当对象中没有as的字段时，使用asField构造as字段*/
    EntityField asField(const char* field);
};

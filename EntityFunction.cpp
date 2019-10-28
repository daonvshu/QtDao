#include "EntityFunction.h"

#include "EntityConditions.h"
#include "EntityField.h"

EntityFunction EntityFunction::limit(int a, int b) {
    auto func = fun();
    func.expressionStr = "limit ?,?";
    func.value(a, b);
    return func;
}

EntityFunction EntityFunction::limit(int a) {
    return limit(0, a);
}

EntityFunction EntityFunction::orderBy(const EntityConditionInterface & field) {
    EntityFunction function;
    if (detach_num == 0) {
        function = fun();
        function.valueList << field.getValueList();
        function.expressionStr = QString("order by %1%2")
            .arg(field.getExpressionStr(), field.isAsc() ? "" : " desc");
        function.detach_num++;
    } else {
        function = *this;
        function.valueList << field.getValueList();
        function.expressionStr += QString(",%1%2")
            .arg(field.getExpressionStr(), field.isAsc() ? "" : " desc");
        function.detach_num++;
    }
    return function;
}

EntityFunction EntityFunction::groupBy(const EntityConditionInterface & field) {
    EntityFunction function;
    if (detach_num == 0) {
        function = fun();
        function.valueList << field.getValueList();
        function.expressionStr = QString("group by %1").arg(field.getExpressionStr());
        function.detach_num++;
    } else {
        function = *this;
        function.valueList << field.getValueList();
        function.expressionStr += QString(",%1").arg(field.getExpressionStr());
        function.detach_num++;
    }
    return function;
}

EntityFunction EntityFunction::having(const EntityConditionInterface & conditions) {
    auto function = fun();
    function.expressionStr = "having " + conditions.getExpressionStr();
    function.valueList << conditions.getValueList();
    return function;
}

EntityConditions & EntityFunction::operator()(EntityConditions & oth) {
    return oth();
}

EntityConditions EntityFunction::operator&&(const EntityConditions & oth) {
    return createCondition() && oth;
}

EntityConditions EntityFunction::operator&&(const EntityFunction & oth) {
    return createCondition() && oth.createCondition();
}

EntityConditions EntityFunction::operator||(const EntityConditions & oth) {
    return createCondition() || oth;
}

EntityConditions EntityFunction::operator||(const EntityFunction & oth) {
    return createCondition() || oth.createCondition();
}

EntityConditions EntityFunction::operator,(const EntityConditions & oth) {
    return (createCondition() , oth);
}

EntityConditions EntityFunction::operator,(const EntityFunction & oth) {
    return (createCondition() , oth.createCondition());
}

EntityFunction & EntityFunction::expStr(const char* str) {
    Q_ASSERT_X(detach_num == 0, "EntityFunction::expStr", u8"使用函数表达式前使用fun()创建新对象");
    expressionStr = str;
    int asIndex = expressionStr.lastIndexOf(" as ");
    if (asIndex != -1) {
        as_field = expressionStr.mid(asIndex + 4).trimmed();
    }
    return *this;
}

EntityFunction & EntityFunction::field(const EntityField & field) {
    expressionStr = expressionStr.arg(field.fieldWithJoinPrefix());
    return *this;
}

EntityFunction & EntityFunction::value(QVariant v) {
    valueList.append(v);
    return *this;
}

QString EntityFunction::getExpressionStr(bool withoutCombineOp) const {
    return expressionStr;
}

QVariantList EntityFunction::getValueList() const {
    return valueList;
}

bool EntityFunction::isAsc() const {
    return asc_value;
}

EntityFunction EntityFunction::asc() {
    EntityFunction f = *this;
    f.asc_value = true;
    return f;
}

EntityFunction EntityFunction::desc() {
    EntityFunction f = *this;
    f.asc_value = false;
    return f;
}

EntityField EntityFunction::asField(const char * field) {
    return field;
}

EntityConditions EntityFunction::createCondition() const {
    EntityField field = "";
    field.kvPairStr = expressionStr;
    field.is_funtion = true;
    field.as_field = as_field;
    field.valueList.clear();
    field.valueList << valueList;
    return EntityConditions().addField(field);
}

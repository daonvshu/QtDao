#include "EntityField.h"

#include "EntityFunction.h"

QString EntityField::getExpressionStr(bool withoutCombineOp) const {
    return fieldWithJoinPrefix();
}

QVariantList EntityField::getValueList() const {
    return valueList;
}

bool EntityField::isAsc() const {
    return asc_value;
}

EntityField EntityField::bindValue(const QString& kvPairStr, const QVariantList& value) {
    EntityField field = *this;
    field.kvPairStr = kvPairStr;
    field.valueList.clear();
    field.valueList.append(value);
    return field;
}

EntityConditions EntityField::setValue(const char * op, const QVariant & v) {
    auto kvStr = QString("%1%2?").arg(fieldWithJoinPrefix(), op);
    QVariantList valueList;
    valueList.append(v);
    return EntityConditions().addField(bindValue(kvStr, valueList));
}

EntityConditions EntityField::setValue(const char * op, const EntityConditionInterface & v) {
    auto kvStr = QString("%1%2%3").arg(fieldWithJoinPrefix(), op, v.getExpressionStr());
    QVariantList valueList;
    valueList << v.getValueList();
    return EntityConditions().addField(bindValue(kvStr, valueList));
}

EntityConditions EntityField::setValueSelf(const char * op, const QVariant & v) {
    auto kvStr = QString("%1=%1%2?").arg(fieldWithJoinPrefix(), op);
    QVariantList valueList;
    valueList.append(v);
    return EntityConditions().addField(bindValue(kvStr, valueList));
}

EntityConditions EntityField::setValueSelf(const char * op, const EntityConditionInterface & v) {
    auto kvStr = QString("%1=%1%2%3").arg(fieldWithJoinPrefix(), op, v.getExpressionStr());
    QVariantList valueList;
    valueList << v.getValueList();
    return EntityConditions().addField(bindValue(kvStr, valueList));
}

QString EntityField::fieldWithJoinPrefix() const {
    return joinPrefix + field;
}

QString EntityField::fieldWithoutJoinPredix() const {
    return field;
}

EntityField EntityField::createCombineOp(const char * op) {
    EntityField field("");
    field.conditionCombineOp = op;
    return field;
}

bool EntityField::isCombineOpType() const {
    return !conditionCombineOp.isEmpty();
}

EntityConditions EntityField::createCondtionWithoutKv() const {
    EntityField field = *this;
    field.kvPairStr = fieldWithJoinPrefix() + (as_field.isEmpty() ? "" : (" as " + as_field));
    return EntityConditions().addField(field);
}

EntityConditions EntityField::in(const QVariantList & v) {
    QString kvStr = QString("%1 in (%2").arg(fieldWithJoinPrefix(), QString("?,").repeated(v.size()));
    kvStr = kvStr.left(kvStr.length() - 1);
    kvStr.append(')');
    return EntityConditions().addField(bindValue(kvStr, v));
}

EntityConditions EntityField::in(const EntityFunction & v) {
    QString kvStr = QString("%1 in (%2)").arg(fieldWithJoinPrefix(), v.expressionStr);
    return EntityConditions().addField(bindValue(kvStr, v.valueList));
}

EntityConditions EntityField::between(const QVariant & a, const QVariant & b) {
    QString kvStr = QString("%1 between ? and ?").arg(fieldWithJoinPrefix());
    QVariantList valueList;
    valueList << a << b;
    return EntityConditions().addField(bindValue(kvStr, valueList));
}

EntityConditions EntityField::between(const EntityField & a, const EntityField & b) {
    QString kvStr = QString("%1 between %2 and %3").arg(fieldWithJoinPrefix(), a.fieldWithJoinPrefix(), b.fieldWithJoinPrefix());
    QVariantList valueList;
    return EntityConditions().addField(bindValue(kvStr, valueList));
}

EntityField EntityField::asc() {
    EntityField field = *this;
    field.asc_value = true;
    return field;
}

EntityField EntityField::desc() {
    EntityField field = *this;
    field.asc_value = false;
    return field;
}

EntityField EntityField::asField(const char * field) {
    EntityField fieldOth = *this;
    fieldOth.as_field = field;
    return fieldOth;
}

EntityConditions EntityField::operator,(const EntityField & oth) {
    return (createCondtionWithoutKv(), oth);
}


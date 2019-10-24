#include "EntityConditions.h"
#include "EntityField.h"
#include "EntityFunction.h"
#include <qvector.h>

EntityConditions & EntityConditions::operator&&(const EntityConditions & oth) {
    this->entityFields.append(EntityField::createCombineOp(" and "));
    this->entityFields.append(oth.entityFields);
    return *this;
}

EntityConditions & EntityConditions::operator&&(const EntityFunction & oth) {
    return *this && oth.createCondition();
}

EntityConditions & EntityConditions::operator||(const EntityConditions & oth) {
    this->entityFields.append(EntityField::createCombineOp(" or "));
    this->entityFields.append(oth.entityFields);
    return *this;
}

EntityConditions & EntityConditions::operator||(const EntityFunction & oth) {
    return *this || oth.createCondition();
}

EntityConditions & EntityConditions::operator,(const EntityConditions & oth) {
    this->entityFields.append(EntityField::createCombineOp(","));
    this->entityFields.append(oth.entityFields);
    return *this;
}

EntityConditions & EntityConditions::operator,(const EntityFunction & oth) {
    return (*this, oth.createCondition());
}

EntityConditions & EntityConditions::operator,(const EntityField & oth) {
    return (*this, oth.createCondtionWithoutKv());
}

EntityConditions & EntityConditions::addField(const EntityField & field) {
    entityFields.append(field);
    return *this;
}

QString EntityConditions::getExpressionStr(bool withoutCombineOp) const {
    QString expressStr = "";
    for (const auto& field : entityFields) {
        if (field.isCombineOpType()) {
            if (!withoutCombineOp) {
                if (!expressStr.isEmpty()) {
                    expressStr += field.conditionCombineOp;
                }
            } else {
                expressStr += ' ';
            }
        } else {
            expressStr += field.kvPairStr;
        }
    }
    return expressStr;
}

QVariantList EntityConditions::getValueList() const {
    QVariantList valueList;
    for (const auto& field : entityFields) {
        valueList.append(field.getValueList());
    }
    return valueList;
}

bool EntityConditions::isAsc() const {
    return false;
}

QStringList EntityConditions::getBindFields(bool withoutJoinPrefix) const {
    QStringList fields;
    for (const auto& field : entityFields) {
        auto fieldStr = withoutJoinPrefix ? field.fieldWithoutJoinPredix() : field.fieldWithJoinPrefix();
        if (!fieldStr.isEmpty()) {
            fields.append(fieldStr);
        } else {
            if (field.is_funtion) {
                if (!field.asField.isEmpty()) {
                    fields << field.asField;
                }
            }
        }
    }
    return fields;
}

const QList<EntityField>& EntityConditions::getBindEntities() {
    return entityFields;
}

void EntityConditions::clearAll() {
    entityFields.clear();
}

EntityConditions & EntityConditions::operator()() {
    entityFields.prepend(EntityField::createCombineOp("("));
    entityFields.append(EntityField::createCombineOp(")"));
    return *this;
}

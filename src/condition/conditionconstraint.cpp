#include "condition/conditionconstraint.h"

QTDAO_BEGIN_NAMESPACE

ConditionConstraint ConditionConstraint::limit(int a, int b) {
    QVariantList values;
    values << a << b;
    return ConditionConstraint(
        values,
        TypeLimit
    );
}

ConditionConstraint ConditionConstraint::limit(int a) {
    QVariantList values;
    values << a;
    return ConditionConstraint(
        values,
        TypeLimit
    );
}

ConditionConstraint ConditionConstraint::having(const EntityCondition& other) {
    ConditionConstraint constraint(QVariantList(), TypeHaving);
    constraint.havingCondition = other;
    return constraint;
}

ConditionConstraint ConditionConstraint::having(const FunctionCondition& condition) {
    ConditionConstraint constraint(QVariantList(), TypeHaving);
    constraint.havingFunction = condition;
    return constraint;
}

ConditionConstraint ConditionConstraint::orderBy(const QString& fieldName, const QString& tbName, bool asc) {
    return ConditionConstraint(
        QVariantList(),
        TypeOrderBy
    ).orderBySave(fieldName, tbName, asc);
}

ConditionConstraint ConditionConstraint::groupBy(const QString& fieldName, const QString& tbName) {
    return ConditionConstraint(
        QVariantList() << fieldName,
        TypeGroupBy
    ).groupBySave(fieldName, tbName);
}

void ConditionConstraint::combine() {
    switch (d->conditionType) {
    case TypeLimit:
        combineLimit();
        break;
    case TypeOrderBy:
        combineOrderBy();
        break;
    case TypeGroupBy:
        combineGroupBy();
        break;
    case TypeHaving:
        combineHaving();
        break;
    default:
        break;
    }
}

void ConditionConstraint::combineLimit() {
    if (d->values.size() == 1) {
        d->combineStr = "limit ?";
    } else if (d->values.size() == 2) {
        d->combineStr = "limit ?,?";
    }
}

void ConditionConstraint::combineOrderBy() {
    d->combineStr = "order by ";
    for (int i = 0; i < d->fields.size(); i++) {
        d->combineStr.append(d->getField(i)).append(',');
    }
    d->combineStr.chop(1);
}

void ConditionConstraint::combineGroupBy() {
    d->combineStr = "group by ";
    for (int i = 0; i < d->fields.size(); i++) {
        d->combineStr.append(d->getField(i)).append(',');
    }
    d->combineStr.chop(1);
}

void ConditionConstraint::combineHaving() {
    if (havingCondition.d) {
        havingCondition.setFieldPrefixGetter(d->fieldPrefixGetter);
        havingCondition.combine();
        d->combineStr = "having " + havingCondition.d->combineStr;
        d->values.append(havingCondition.d->values);
    }
    if (havingFunction.d) {
        havingFunction.setFieldPrefixGetter(d->fieldPrefixGetter);
        havingFunction.combine();
        d->combineStr = "having " + havingFunction.d->combineStr;
        d->values.append(havingFunction.d->values);
    }
}

QVariantList ConditionConstraint::getValues() {
    if (d->conditionType == TypeOrderBy || d->conditionType == TypeGroupBy) {
        return QVariantList();
    }
    return EntityCondition::getValues();
}

ConditionConstraint& ConditionConstraint::orderBySave(const QString& fieldName, const QString& tbName, bool asc) {
    QString tmpValue = fieldName;
    if (!asc) {
        tmpValue += " desc";
    }
    d->fields << FieldInfo {tmpValue, tbName};
    return *this;
}

ConditionConstraint& ConditionConstraint::groupBySave(const QString& fieldName, const QString& tbName) {
    d->fields << FieldInfo { fieldName, tbName };
    return *this;
}

QTDAO_END_NAMESPACE
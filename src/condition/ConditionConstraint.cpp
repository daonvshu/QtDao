#include "ConditionConstraint.h"

ConditionConstraint ConditionConstraint::limit(int a, int b) {
    QVariantList values;
    values << a << b;
    return ConditionConstraint(
        "",
        values,
        TypeLimit
    );
}

ConditionConstraint ConditionConstraint::limit(int a) {
    QVariantList values;
    values << a;
    return ConditionConstraint(
        "",
        values,
        TypeLimit
    );
}

ConditionConstraint ConditionConstraint::orderBy(const QString& fieldName, bool asc) {
    return ConditionConstraint(
        "",
        QVariantList(),
        TypeOrderBy
    ).orderByNext(fieldName, asc);
}

ConditionConstraint ConditionConstraint::groupBy(const QString& fieldName) {
    return ConditionConstraint(
        "",
        QVariantList() << fieldName,
        TypeGroupBy
    );
}

void ConditionConstraint::combine(const QString& fieldPrefix) {
    switch (d->conditionType) {
    case TypeLimit:
        combineLimit(fieldPrefix);
        break;
    case TypeOrderBy:
        combineOrderBy(fieldPrefix);
        break;
    case TypeGroupBy:
        combineGroupBy(fieldPrefix);
        break;
    case TypeHaving:
        break;
    default:
        break;
    }
}

void ConditionConstraint::combineLimit(const QString& fieldPrefix) {
    if (d->values.size() == 1) {
        d->combineStr = "limit ?";
    } else if (d->values.size() == 2) {
        d->combineStr = "limit ?,?";
    }
}

void ConditionConstraint::combineOrderBy(const QString& fieldPrefix) {
    d->combineStr = "order by ";
    for (const auto& v : d->values) {
        d->combineStr.append(fieldPrefix).append(v.toString()).append(',');
    }
    d->combineStr.chop(1);
}

void ConditionConstraint::combineGroupBy(const QString& fieldPrefix) {
    d->combineStr = "group by ";
    for (const auto& v : d->values) {
        d->combineStr.append(fieldPrefix).append(v.toString()).append(',');
    }
    d->combineStr.chop(1);
}

QVariantList ConditionConstraint::getValues() {
    if (d->conditionType == TypeOrderBy || d->conditionType == TypeGroupBy) {
        return QVariantList();
    }
    return EntityCondition::getValues();
}

ConditionConstraint& ConditionConstraint::orderByNext(const QString& fieldName, bool asc) {
    QString tmpValue = fieldName;
    if (!asc) {
        tmpValue += " desc";
    }
    d->values << tmpValue;
    return *this;
}

ConditionConstraint& ConditionConstraint::groupByNext(const QString& fieldName) {
    d->values << fieldName;
    return *this;
}

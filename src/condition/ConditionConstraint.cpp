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
    switch (conditionType) {
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
    if (values.size() == 1) {
        combineStr = "limit ?";
    } else if (values.size() == 2) {
        combineStr = "limit ?,?";
    }
}

void ConditionConstraint::combineOrderBy(const QString& fieldPrefix) {
    combineStr = "order by ";
    for (const auto& v : values) {
        combineStr.append(fieldPrefix).append(v.toString()).append(',');
    }
    combineStr.chop(1);
}

void ConditionConstraint::combineGroupBy(const QString& fieldPrefix) {
    combineStr = "group by ";
    for (const auto& v : values) {
        combineStr.append(fieldPrefix).append(v.toString()).append(',');
    }
    combineStr.chop(1);
}

ConditionConstraint& ConditionConstraint::orderByNext(const QString& fieldName, bool asc) {
    QString tmpValue = fieldName;
    if (!asc) {
        tmpValue += " desc";
    }
    values << tmpValue;
    return *this;
}

ConditionConstraint& ConditionConstraint::groupByNext(const QString& fieldName) {
    values << fieldName;
    return *this;
}

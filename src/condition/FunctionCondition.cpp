#include "FunctionCondition.h"

FunctionCondition::FunctionCondition(const QString& expressions) {
    d = new FunctionConditionData;
    d->expressions = expressions;
}

void FunctionCondition::combine(const QString& fieldPrefix) {
    QString str = d->expressions;
    for (const auto& f : d->fieldNames) {
        str = str.arg(fieldPrefix + f);
    }
    d->combineStr = str;
}
#include "condition/functioncondition.h"

FunctionCondition::FunctionCondition() {
}

FunctionCondition& FunctionCondition::value(QVariant v) {
    d->values << v;
    return *this;
}

FunctionCondition::FunctionCondition(const QString& expressions) {
    d = new FunctionConditionData;
    d->expressions = expressions;
}

void FunctionCondition::setFieldPrefixGetter(std::function<QString(const QString&)> prefixGetter) {
    d->fieldPrefixGetter = prefixGetter;
}

void FunctionCondition::combine() {
    QString str = d->expressions;
    for (int i = 0; i < d->fields.size(); i++) {
        str = str.arg(d->getField(i));
    }
    d->combineStr = str;
}

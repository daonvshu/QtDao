#include "condition/conditionconstraint.h"

QTDAO_BEGIN_NAMESPACE

LimitConstraintConnector::LimitConstraintConnector(int rows) {
    d->values << rows;
}

LimitConstraintConnector::LimitConstraintConnector(int offset, int rows) {
    d->values << offset << rows;
}

void LimitConstraintConnector::combine() {
    d->connectedStr = "limit ?";
    if (d->values.size() > 1) {
        d->connectedStr += ",?";
    }
}

bool LimitConstraintConnector::isEmpty() {
    return d->values.isEmpty();
}

QVariantList LimitConstraintConnector::getValueList() {
    return d->values;
}

void OrderByConstraintConnector::combine() {
    d->connectedStr = "order by ";
    for (int i = 0; i < d->fields.size(); i++) {
        d->connectedStr.append(getField(i)).append(',');
    }
    d->connectedStr.chop(1);
}

void GroupByConstraintConnector::combine() {
    d->connectedStr = "group by ";
    for (int i = 0; i < d->fields.size(); i++) {
        d->connectedStr.append(getField(i)).append(',');
    }
    d->connectedStr.chop(1);
}

QTDAO_END_NAMESPACE
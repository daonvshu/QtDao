#include "condition/conditionconstraint.h"

QTDAO_BEGIN_NAMESPACE

LimitConstraintConnector::LimitConstraintConnector(int rows) {
    values << rows;
}

LimitConstraintConnector::LimitConstraintConnector(int offset, int rows) {
    values << offset << rows;
}

void LimitConstraintConnector::combine() {
    d->connectedStr = "limit ?";
    if (values.size() > 1) {
        d->connectedStr += ",?";
    }
}

QVariantList LimitConstraintConnector::getValueList() {
    return values;
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
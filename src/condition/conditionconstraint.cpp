#include "condition/conditionconstraint.h"

QTDAO_BEGIN_NAMESPACE

LimitConstraintConnector::LimitConstraintConnector(int rows) {
    values << rows;
}

LimitConstraintConnector::LimitConstraintConnector(int offset, int rows) {
    values << offset << rows;
}

void LimitConstraintConnector::combine() {
    connectedStr = "limit ?";
    if (values.size() > 1) {
        connectedStr += ",?";
    }
}

QVariantList LimitConstraintConnector::getValueList() {
    return values;
}

void OrderByConstraintConnector::combine() {
    connectedStr = "order by ";
    for (int i = 0; i < fields.size(); i++) {
        connectedStr.append(getField(i)).append(',');
    }
    connectedStr.chop(1);
}

void GroupByConstraintConnector::combine() {
    connectedStr = "group by ";
    for (int i = 0; i < fields.size(); i++) {
        connectedStr.append(getField(i)).append(',');
    }
    connectedStr.chop(1);
}

QTDAO_END_NAMESPACE
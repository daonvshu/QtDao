#pragma once

#include <qobject.h>

class EntityConditionInterface {
public:
    virtual QString getExpressionStr(bool withoutCombineOp = false) const = 0;
    virtual QVariantList getValueList() const = 0;
    virtual bool isAsc() const = 0;
};
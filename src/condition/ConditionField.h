#pragma once

#include <qobject.h>

class ConditionField {
public:
    ConditionField(const char* fieldName) : name(fieldName) {}

private:
    QString name;
};


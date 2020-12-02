#pragma once

#include <qsharedpointer.h>
#include <qstring.h>
#include <qvariant.h>

#include "EntityConditionData.h"

struct FunctionConditionData : public EntityConditionData {
    QString expressions;
};
#pragma once

#include "../global.h"

#include <qloggingcategory.h>

QTDAO_BEGIN_NAMESPACE

typedef const QLoggingCategory& (*LoggingCategoryPtr)();

QTDAO_END_NAMESPACE

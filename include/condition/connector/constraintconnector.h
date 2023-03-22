#pragma once

#include "entityconnector.h"

#include <qobject.h>
#include <qvariant.h>

QTDAO_BEGIN_NAMESPACE

class ConstraintConnector : EntityConnector {
public:
    using EntityConnector::addField;

protected:
    using EntityConnector::getField;
    using EntityConnector::fields;
    using EntityConnector::connectedStr;
};

QTDAO_END_NAMESPACE
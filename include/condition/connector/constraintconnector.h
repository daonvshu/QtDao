#pragma once

#include "entityconnector.h"

#include <qobject.h>
#include <qvariant.h>

QTDAO_BEGIN_NAMESPACE

class ConstraintConnector : EntityConnector {
public:
    using EntityConnector::addField;

    //only used for single inheritance structure!
    Connectable* ptr() {
        return this;
    }

protected:
    using Connectable::getField;
    using Connectable::getEntityFieldInfo;
    using Connectable::fields;
    using Connectable::connectedStr;
};

QTDAO_END_NAMESPACE
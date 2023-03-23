#pragma once

#include "option/debugbuilder.h"
#include "option/filterbuilder.h"
#include "option/setbuilder.h"

#include "../macro/macro.h"

#include "../query/update.h"

QTDAO_BEGIN_NAMESPACE

template<typename E>
class UpdateBuilder
        : public DebugBuilder<UpdateBuilder<E>>
        , public FilterBuilder<UpdateBuilder<E>>
        , public SetBuilder<UpdateBuilder<E>>
{
public:
    Update <E> build() {
        return Update<E>(*this);
    }
};

QTDAO_END_NAMESPACE
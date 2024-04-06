#pragma once

#include "option/debugbuilder.h"
#include "option/filterbuilder.h"
#include "option/sessionbuilder.h"

#include "../macro/macro.h"

#include "../query/delete.h"

QTDAO_BEGIN_NAMESPACE

template<typename E>
class DeleteBuilder
    : public DebugBuilder<DeleteBuilder<E>>
    , public FilterBuilder<DeleteBuilder<E>>
    , public SessionBuilder<DeleteBuilder<E>>
{
public:
    using SessionBuilder<DeleteBuilder<E>>::SessionBuilder;
    
    Delete<E> build() {
        return Delete<E>(*this);
    }
};

QTDAO_END_NAMESPACE
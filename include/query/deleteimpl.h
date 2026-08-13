#pragma once

#include "basequery.h"
#include "reader/entityreaderinterface.h"
#include "reader/builderreaderinterface.h"

#include <functional>

QTDAO_BEGIN_NAMESPACE

class DeleteImpl
        : protected BaseQuery
        , protected virtual EntityReaderInterface
        , protected virtual BuilderReaderInterface
{
public:
    using BaseQuery::BaseQuery;

protected:
    void buildDeleteByFilterSqlStatement();

    void buildDeleteEntitiesCondition(const std::function<QVariantList(const QString&)>& fieldColValuesReader, bool batchMode);

    template<typename E>
    void buildDeleteEntitiesCondition(const QList<E>& entities, bool batchMode);
};

template<typename E>
inline void DeleteImpl::buildDeleteEntitiesCondition(const QList<E> &entities, bool batchMode) {
    buildDeleteEntitiesCondition([&](const QString& fieldName) {
        return listMap<QVariant, E>(entities, [&](const E& entity) {
            return E::Tool::getValueByName(entity, fieldName);
        });
    }, batchMode);
}

QTDAO_END_NAMESPACE

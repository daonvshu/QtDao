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

    void buildDeleteEntitiesCondition(const std::function<QVariantList(const QString&)>& fieldColValuesReader);

    template<typename E>
    void buildDeleteEntitiesCondition(const QList<E>& entities);
};

template<typename E>
inline void DeleteImpl::buildDeleteEntitiesCondition(const QList<E> &entities) {
    buildDeleteEntitiesCondition([&](const QString& fieldName) {
        return listMap<QVariant, E>(entities, [&](const E& entity) {
            return E::Tool::getValueByName(entity, fieldName);
        });
    });
}

QTDAO_END_NAMESPACE

#pragma once

#include "basequery.h"
#include "entityreaderinterface.h"

QTDAO_BEGIN_NAMESPACE

class UpdateImpl : protected BaseQuery, protected virtual EntityReaderInterface {
public:
    using BaseQuery::BaseQuery;

protected:
    void buildUpdateBySetSqlStatement();

    void bindUpdateEntitiesCondition(const std::function<QVariantList(const QString&)>& fieldColValuesReader);

    template<typename E>
    void bindUpdateEntitiesCondition(const QList<E>& entities);
};

template<typename E>
inline void UpdateImpl::bindUpdateEntitiesCondition(const QList<E> &entities) {
    bindUpdateEntitiesCondition([&](const QString& fieldName) {
        return listMap<QVariant, E>(entities, [&](const E& entity) {
            return E::Tool::getValueByName(entity, fieldName);
        });
    });
}


QTDAO_END_NAMESPACE

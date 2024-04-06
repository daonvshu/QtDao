#pragma once

#include "deleteimpl.h"

#include "../macro/macro.h"

QTDAO_BEGIN_NAMESPACE

template<typename E>
class DeleteBuilder;

template<typename E>
class Delete
        : EntityReaderProvider<E>
        , BuilderReaderProvider<DeleteBuilder, E>
        , DeleteImpl
{
public:
    /**
     * delete by filter condition
     */
    void deleteBy();

    /**
     * delete by filter condition in batches
     */
    void deleteBatch();

    /**
     * delete object by primary key
     * @param entity
     */
    void deleteBy(const E& entity);

    /**
     * delete object by primary key in batches
     * @param entities
     */
    void deleteBatch(const QList<E>& entities);

private:
    BASE_QUERY_CONSTRUCTOR_DECLARE(Delete)
};

template<typename E>
inline void Delete<E>::deleteBy() {
    buildDeleteByFilterSqlStatement();
    setDebug(this->builder);
    exec(getSessionId());
}

template<typename E>
inline void Delete<E>::deleteBatch() {
    buildDeleteByFilterSqlStatement();
    setDebug(this->builder);
    execBatch(getSessionId());
}

template<typename E>
inline void Delete<E>::deleteBy(const E& entity) {
    buildDeleteEntitiesCondition(QList<E>() << entity);
    return deleteBy();
}

template<typename E>
inline void Delete<E>::deleteBatch(const QList<E>& entities) {
    if (entities.isEmpty()) {
        return;
    }
    buildDeleteEntitiesCondition(entities);
    return deleteBatch();
}
QTDAO_END_NAMESPACE
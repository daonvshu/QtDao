#pragma once

#include "deleteimpl.h"

#include "../macro/macro.h"

QTDAO_BEGIN_NAMESPACE

template<typename E>
class DeleteBuilder;

template<typename E>
class Delete : EntityReaderProvider<E>, DeleteImpl {
public:
    /// <summary>
    /// delete by filter condition
    /// </summary>
    void deleteBy();

    /// <summary>
    /// delete by filter condition in batches
    /// </summary>
    void deleteBatch();

    /// <summary>
    /// delete object by primary key
    /// </summary>
    /// <param name="entity"></param>
    void deleteBy(const E& entity);

    /// <summary>
    /// delete object by primary key in batches
    /// </summary>
    /// <param name="entities"></param>
    void deleteBatch(const QList<E>& entities);

private:
    BASE_QUERY_CONSTRUCTOR_DECLARE(Delete)
};

template<typename E>
inline void Delete<E>::deleteBy() {
    buildDeleteByFilterSqlStatement();
    exec();
}

template<typename E>
inline void Delete<E>::deleteBatch() {
    buildDeleteByFilterSqlStatement();
    execBatch();
}

template<typename E>
inline void Delete<E>::deleteBy(const E& entity) {
    buildDeleteEntitiesCondition(QList<E>() << entity);
    return deleteBy();
}

template<typename E>
inline void Delete<E>::deleteBatch(const QList<E>& entities) {
    buildDeleteEntitiesCondition(entities);
    return deleteBatch();
}
QTDAO_END_NAMESPACE
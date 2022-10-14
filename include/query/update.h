#pragma once

#include "updateimpl.h"

#include "../macro/macro.h"

QTDAO_BEGIN_NAMESPACE

template<typename E>
class UpdateBuilder;

template<typename E>
class Update : EntityReaderProvider<E>, UpdateImpl {
public:
    /// <summary>
    /// using the SET condition to update a record
    /// </summary>
    /// <returns>effect rows</returns>
    int update();

    /// <summary>
    /// using the SET condition to update a record in batches 
    /// </summary>
    /// <returns>effect rows</returns>
    int updateBatch();

    /// <summary>
    /// update by object's primary key
    /// </summary>
    /// <param name="entity"></param>
    /// <returns>effect rows</returns>
    int update(const E& entity);

    /// <summary>
    /// update by object's primary key in batches 
    /// </summary>
    /// <param name="entities"></param>
    /// <returns>effect rows</returns>
    int updateBatch(const QList<E>& entities);

private:
    BASE_QUERY_CONSTRUCTOR_DECLARE(Update)
};

template<typename E>
inline int Update<E>::update() {
    buildUpdateBySetSqlStatement();
    auto query = exec();
    return query.numRowsAffected();
}

template<typename E>
inline int Update<E>::updateBatch() {
    buildUpdateBySetSqlStatement();
    auto query = execBatch();
    return query.numRowsAffected();
}

template<typename E>
inline int Update<E>::update(const E& entity) {
    bindUpdateEntitiesCondition(QList<E>() << entity);
    return update();
}

template<typename E>
inline int Update<E>::updateBatch(const QList<E>& entities) {
    bindUpdateEntitiesCondition(entities);
    return updateBatch();
}

QTDAO_END_NAMESPACE

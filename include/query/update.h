#pragma once

#include "updateimpl.h"

#include "../macro/macro.h"

QTDAO_BEGIN_NAMESPACE

template<typename E>
class UpdateBuilder;

template<typename E>
class Update
        : EntityReaderProvider<E>
        , BuilderReaderProvider<UpdateBuilder, E>
        , UpdateImpl
{
public:
    /**
     * using the SET condition to update a record
     * @return effect rows
     */
    int update();

    /**
     * using the SET condition to update a record in batches
     * @return effect rows
     */
    int updateBatch();

    /**
     * update by object's primary key
     * @param entity
     * @return effect rows
     */
    int update(const E& entity);

    /**
     * update by object's primary key in batches
     * @param entities
     * @return effect rows
     */
    int updateBatch(const QList<E>& entities);

private:
    BASE_QUERY_CONSTRUCTOR_DECLARE(Update)
};

template<typename E>
inline int Update<E>::update() {
    buildUpdateBySetSqlStatement();
    setDebug(this->builder);
    auto query = exec();
    return query.numRowsAffected();
}

template<typename E>
inline int Update<E>::updateBatch() {
    buildUpdateBySetSqlStatement();
    setDebug(this->builder);
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
    if (entities.isEmpty()) {
        return 0;
    }
    bindUpdateEntitiesCondition(entities);
    return updateBatch();
}

QTDAO_END_NAMESPACE

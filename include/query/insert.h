#pragma once

#include "insertimpl.h"

#include "../macro/macro.h"

#include <qvector.h>

QTDAO_BEGIN_NAMESPACE

template<typename E>
class InsertBuilder;

template<typename E>
class Insert
        : EntityReaderProvider<E>
        , BuilderReaderProvider<InsertBuilder, E>
        , InsertImpl
{
public:
    /**
     * using the SET condition to insert a record, can be inserted in batches
     */
    void insert();

    /**
     * using the SET condition to insert a record, can be inserted in batches,
     * update if a unique constraint exists, otherwise insert
     */
    void insertOrReplace() {
        insertMode = InsertMode::INSERT_OR_REPLACE;
        insert();
    }

    /**
     * using the SET condition to insert a record, can be inserted in batches,
     * ignore when insert conflict
     */
    void insertOrIgnore() {
        insertMode = InsertMode::INSERT_OR_IGNORE;
        insert();
    }

    /**
     * insert an object instance and set the ID back to the object after successful insertion
     * @param entity
     */
    void insert(E& entity);

    /**
     * insert an object instance and set the ID back to the object after successful insertion,
     * update if a unique constraint exists, otherwise insert
     * @param entity
     */
    void insertOrReplace(E& entity) {
        insertMode = InsertMode::INSERT_OR_REPLACE;
        insert(entity);
    }

    /**
     * insert an object instance and set the ID back to the object after successful insertion,
     * ignore when insert conflict
     * @param entity
     */
    void insertOrIgnore(E& entity) {
        insertMode = InsertMode::INSERT_OR_IGNORE;
        insert(entity);
    }

    /**
     * insert objects in batches,
     * use 'execbatch'
     * @param entities
     */
    void insert(const QList<E>& entities);

    /**
     * insert objects in batches, use 'execbatch',
     * update if a unique constraint exists, otherwise insert
     * @param entities
     */
    void insertOrReplace(const QList<E>& entities) {
        insertMode = InsertMode::INSERT_OR_REPLACE;
        insert(entities);
    }

    /**
     * insert objects in batches, use 'execbatch',
     * ignore when insert conflict
     * @param entities
     */
    void insertOrIgnore(const QList<E>& entities) {
        insertMode = InsertMode::INSERT_OR_IGNORE;
        insert(entities);
    }

    /**
     * insert objects in batches,
     * use 'exec'，the values list connect by string(warning: sql statement length limit),
     * insert into E (xx, xx) values(xx,xx), (xx, xx), (xx, xx)
     * @param entities
     */
    void insert2(const QList<E>& entities);

    /**
     * insert objects in batches,
     * use 'exec'，the values list connect by string(warning: sql statement length limit),
     * insert into E (xx, xx) values(xx,xx), (xx, xx), (xx, xx),
     * update if a unique constraint exists, otherwise insert
     * @param entities
     */
    void insert2OrReplace(const QList<E>& entities) {
        insertMode = InsertMode::INSERT_OR_REPLACE;
        insert2(entities);
    }

    /**
     * insert objects in batches,
     * use 'exec'，the values list connect by string(warning: sql statement length limit),
     * insert into E (xx, xx) values(xx,xx), (xx, xx), (xx, xx),
     * ignore when insert conflict
     * @param entities
     */
    void insert2OrIgnore(const QList<E>& entities) {
        insertMode = InsertMode::INSERT_OR_IGNORE;
        insert2(entities);
    }

private:
    BASE_QUERY_CONSTRUCTOR_DECLARE(Insert)
};

template<typename E>
inline void Insert<E>::insert() {
    bool operateBatch = buildInsertBySetSqlStatement();
    setDebug(this->builder);
    operateBatch ? execBatch(getSessionId()) : exec(getSessionId());
}

template<typename E>
inline void Insert<E>::insert(E& entity) {
    auto sqlStatement = buildInsertObjectSqlStatement();
    auto values = EntityReaderProvider<E>::getValueWithoutAutoIncrement(entity);
    setSqlQueryStatement(sqlStatement, values);

    setDebug(this->builder);
    QSqlQuery query = exec(getSessionId());
    EntityReaderProvider<E>::bindAutoIncrementId(entity, query.lastInsertId());
}

template<typename E>
inline void Insert<E>::insert(const QList<E>& entities) {
    if (entities.isEmpty()) {
        return;
    }
    auto sqlStatement = buildInsertObjectsSqlStatement();
    QVector<QVariantList> values(fieldSize());
    int usedValueSize = 0;
    for (const auto& entity : entities) {
        auto v = EntityReaderProvider<E>::getValueWithoutAutoIncrement(entity);
        usedValueSize = v.size();
        for (int i = 0; i < v.size(); i++) {
            values[i] << v.at(i);
        }
    }
    QVariantList tagValues;
    for (int i = 0; i < usedValueSize; i++) {
        tagValues << QVariant(values.at(i));
    }
    setSqlQueryStatement(sqlStatement, tagValues);
    setDebug(this->builder);
    execBatch(getSessionId());
}

template<typename E>
inline void Insert<E>::insert2(const QList<E>& entities) {
    int entitySize = entities.size();
    if (entitySize == 0) {
        return;
    }
    auto sqlStatement = buildInsertObjects2SqlStatement(entitySize);
    QVariantList values;
    for (const auto& entity: entities) {
        values << EntityReaderProvider<E>::getValueWithoutAutoIncrement(entity);
    }
    setSqlQueryStatement(sqlStatement, values);
    setDebug(this->builder);
    exec(getSessionId());
}

QTDAO_END_NAMESPACE
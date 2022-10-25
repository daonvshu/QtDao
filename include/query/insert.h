#pragma once

#include "insertimpl.h"

#include "../macro/macro.h"

#include <qvector.h>

QTDAO_BEGIN_NAMESPACE

template<typename E>
class InsertBuilder;

template<typename E>
class Insert : EntityReaderProvider<E>, InsertImpl {
public:
    /// <summary>
    /// using the SET condition to insert a record, can be inserted in batches
    /// </summary>
    void insert();

    /// <summary>
    /// using the SET condition to insert a record, can be inserted in batches 
    /// update if a unique constraint exists, otherwise insert
    /// </summary>
    void insertOrReplace() {
        insertMode = InsertMode::INSERT_OR_REPLACE;
        insert();
    }

    /// <summary>
    /// using the SET condition to insert a record, can be inserted in batches
    /// ignore when insert conflict
    /// </summary>
    void insertOrIgnore() {
        insertMode = InsertMode::INSERT_OR_IGNORE;
        insert();
    }

    /// <summary>
    /// insert an object instance and set the ID back to the object after successful insertion
    /// </summary>
    /// <param name="entity"></param>
    void insert(E& entity);

    /// <summary>
    /// insert an object instance and set the ID back to the object after successful insertion 
    /// update if a unique constraint exists, otherwise insert
    /// </summary>
    /// <param name="entity"></param>
    void insertOrReplace(E& entity) {
        insertMode = InsertMode::INSERT_OR_REPLACE;
        insert(entity);
    }

    /// <summary>
    /// insert an object instance and set the ID back to the object after successful insertion
    /// ignore when insert conflict
    /// </summary>
    /// <param name="entity"></param>
    void insertOrIgnore(E& entity) {
        insertMode = InsertMode::INSERT_OR_IGNORE;
        insert(entity);
    }

    /// <summary>
    /// insert objects in batches 
    /// use 'execbatch'
    /// </summary>
    /// <param name="entities"></param>
    void insert(const QList<E>& entities);

    /// <summary>
    /// insert objects in batches 
    /// use 'execbatch' 
    /// update if a unique constraint exists, otherwise insert
    /// </summary>
    /// <param name="entities"></param>
    void insertOrReplace(const QList<E>& entities) {
        insertMode = InsertMode::INSERT_OR_REPLACE;
        insert(entities);
    }

    /// <summary>
    /// insert objects in batches
    /// use 'execbatch'
    /// ignore when insert conflict
    /// </summary>
    /// <param name="entities"></param>
    void insertOrIgnore(const QList<E>& entities) {
        insertMode = InsertMode::INSERT_OR_IGNORE;
        insert(entities);
    }

    /// <summary>
    /// insert objects in batches 
    /// use 'exec'，the values list connect by string(warning: sql statement length limit)
    /// insert into E (xx, xx) values(xx,xx), (xx, xx), (xx, xx)
    /// </summary>
    /// <param name="entities"></param>
    void insert2(const QList<E>& entities);

    /// <summary>
    /// insert objects in batches
    /// use 'exec'，the values list connect by string(warning: sql statement length limit)
    /// insert into E (xx, xx) values(xx,xx), (xx, xx), (xx, xx)
    /// update if a unique constraint exists, otherwise insert
    /// </summary>
    /// <param name="entities"></param>
    void insert2OrReplace(const QList<E>& entities) {
        insertMode = InsertMode::INSERT_OR_REPLACE;
        insert2(entities);
    }

    /// <summary>
    /// insert objects in batches
    /// use 'exec'，the values list connect by string(warning: sql statement length limit)
    /// insert into E (xx, xx) values(xx,xx), (xx, xx), (xx, xx)
    /// ignore when insert conflict
    /// </summary>
    /// <param name="entities"></param>
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
    operateBatch ? execBatch() : exec();
}

template<typename E>
inline void Insert<E>::insert(E& entity) {
    auto sqlStatement = buildInsertObjectSqlStatement();
    auto values = EntityReaderProvider<E>::getValueWithoutAutoIncrement(entity);
    setSqlQueryStatement(sqlStatement, values);

    QSqlQuery query = exec();
    EntityReaderProvider<E>::bindAutoIncrementId(entity, query.lastInsertId());
}

template<typename E>
inline void Insert<E>::insert(const QList<E>& entities) {
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
    execBatch();
}

template<typename E>
inline void Insert<E>::insert2(const QList<E>& entities) {
    int entitySize = entities.size();
    Q_ASSERT(entitySize != 0);
    auto sqlStatement = buildInsertObjects2SqlStatement(entitySize);
    QVariantList values;
    for (const auto& entity: entities) {
        values << EntityReaderProvider<E>::getValueWithoutAutoIncrement(entity);
    }
    setSqlQueryStatement(sqlStatement, values);
    exec();
}

QTDAO_END_NAMESPACE
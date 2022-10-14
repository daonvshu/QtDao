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
        this->insertOrRp = true;
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
        this->insertOrRp = true;
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
        this->insertOrRp = true;
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
        this->insertOrRp = true;
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
    auto values = getValueWithoutAutoIncrement(entity);
    setSqlQueryStatement(buildInsertObjectSqlStatement(), values);

    QSqlQuery query = exec();
    bindAutoIncrementId(entity, query.lastInsertId());
}

template<typename E>
inline void Insert<E>::insert(const QList<E>& entities) {
    auto sqlStatement = buildInsertObjectsSqlStatement();
    QVector<QVariantList> values(fieldSize());
    int usedValueSize = 0;
    for (const auto& entity : entities) {
        auto v = getValueWithoutAutoIncrement(entity);
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
    auto values = listMap<QVariant, E>(entities, &E::Tool::getValueWithoutAutoIncrement);
    setSqlQueryStatement(sqlStatement, values);
    exec();
}

QTDAO_END_NAMESPACE
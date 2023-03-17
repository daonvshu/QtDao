#pragma once

#include "upsertimpl.h"

#include "../macro/macro.h"

QTDAO_BEGIN_NAMESPACE

template<typename E>
class UpsertBuilder;

template<typename E>
class Upsert
        : EntityReaderProvider<E>
        , BuilderReaderProvider<UpsertBuilder, E>
        , UpsertImpl
{
public:
    /**
     * using the SET condition to insert a record, can be inserted in batches
     */
    void insert();

    /**
     * insert an object instance and set the ID back to the object after successful insertion
     * @param entity
     */
    void insert(E& entity);

    /**
     * insert objects in batches, use 'execbatch'
     * @param entities
     */
    void insert(const QList<E>& entities);

private:
    BASE_QUERY_CONSTRUCTOR_DECLARE(Upsert)
};

template<typename E>
void Upsert<E>::insert() {
    bool operateBatch = buildInsertBySetSqlStatement();
    operateBatch ? execBatch() : exec();
}

template<typename E>
void Upsert<E>::insert(E &entity) {

    auto values = EntityReaderProvider<E>::getValueWithoutAutoIncrement(entity);
    auto fields = getFieldsWithoutAutoIncrement();
    auto sqlStatement = buildInsertStatement(fields, [&](const QString& field) {
        return values.at(fields.indexOf(field));
    });
    values.append(this->values);
    setSqlQueryStatement(sqlStatement, values);
    setDebug(this->builder);

    QSqlQuery query = exec();
    EntityReaderProvider<E>::bindAutoIncrementId(entity, query.lastInsertId());
}

template<typename E>
void Upsert<E>::insert(const QList<E> &entities) {

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

    auto fields = getFieldsWithoutAutoIncrement();
    auto sqlStatement = buildInsertStatement(fields, [&](const QString& field) {
        return tagValues.at(fields.indexOf(field));
    });

    tagValues.append(this->values);
    setSqlQueryStatement(sqlStatement, tagValues);
    setDebug(this->builder);

    execBatch();
}

QTDAO_END_NAMESPACE
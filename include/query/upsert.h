#pragma once

#include "upsertimpl.h"

#include "../macro/macro.h"

QTDAO_BEGIN_NAMESPACE

template<typename E>
class UpsertBuilder;

template<typename E>
class Upsert : EntityReaderProvider<E>, UpsertImpl {
public:
    /// <summary>
    /// using the SET condition to insert a record, can be inserted in batches
    /// </summary>
    void insert();

    /// <summary>
    /// insert an object instance and set the ID back to the object after successful insertion
    /// </summary>
    /// <param name="entity"></param>
    void insert(E& entity);

    /// <summary>
    /// insert objects in batches
    /// use 'execbatch'
    /// </summary>
    /// <param name="entities"></param>
    void insert(const QList<E>& entities);

private:
    BASE_QUERY_CONSTRUCTOR_DECLARE(Upsert)

    void applyCols() {
        auto builderPtr = dynamic_cast<UpsertBuilder<E>*>(builder);
        conflictCols = builderPtr->conflictCols;
        updateCols = builderPtr->conflictCols;
    }
};

template<typename E>
void Upsert<E>::insert() {
    applyCols();
    bool operateBatch = buildInsertBySetSqlStatement();
    operateBatch ? execBatch() : exec();
}

template<typename E>
void Upsert<E>::insert(E &entity) {
    applyCols();

    auto values = EntityReaderProvider<E>::getValueWithoutAutoIncrement(entity);
    auto fields = getFieldsWithoutAutoIncrement();
    auto sqlStatement = buildInsertStatement(fields, [&](const QString& field) {
        return values.at(fields.indexOf(field));
    });
    values.append(this->values);
    setSqlQueryStatement(sqlStatement, values);

    QSqlQuery query = exec();
    EntityReaderProvider<E>::bindAutoIncrementId(entity, query.lastInsertId());
}

template<typename E>
void Upsert<E>::insert(const QList<E> &entities) {
    applyCols();

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

    values.append(this->values);
    setSqlQueryStatement(sqlStatement, tagValues);
    execBatch();
}

QTDAO_END_NAMESPACE
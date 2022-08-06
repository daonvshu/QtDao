#pragma once

#include "basequery.h"

#include "../macro/macro.h"

#include "../dbloader.h"

template<typename E>
class InsertBuilder;

template<typename E>
class Insert : BaseQuery {
public:
    /// <summary>
    /// using the SET condition to insert a record, can be inserted in batches
    /// </summary>
    /// <returns></returns>
    bool insert();

    /// <summary>
    /// using the SET condition to insert a record, can be inserted in batches 
    /// update if a unique constraint exists, otherwise insert
    /// </summary>
    /// <returns></returns>
    bool insertOrReplace() {
        insertOrRp = true;
        return insert();
    }

    /// <summary>
    /// insert an object instance and set the ID back to the object after successful insertion
    /// </summary>
    /// <param name="entity"></param>
    /// <returns></returns>
    bool insert(E& entity);

    /// <summary>
    /// insert an object instance and set the ID back to the object after successful insertion 
    /// update if a unique constraint exists, otherwise insert
    /// </summary>
    /// <param name="entity"></param>
    /// <returns></returns>
    bool insertOrReplace(E& entity) {
        insertOrRp = true;
        return insert(entity);
    }

    /// <summary>
    /// insert objects in batches 
    /// use 'execbatch'
    /// </summary>
    /// <param name="entities"></param>
    /// <returns></returns>
    bool insert(const QList<E>& entities);

    /// <summary>
    /// insert objects in batches 
    /// use 'execbatch' 
    /// update if a unique constraint exists, otherwise insert
    /// </summary>
    /// <param name="entities"></param>
    /// <returns></returns>
    bool insertOrReplace(const QList<E>& entities) {
        insertOrRp = true;
        return insert(entities);
    }

    /// <summary>
    /// insert objects in batches 
    /// use 'exec'，the values list connect by string(warning: sql statement length limit)
    /// insert into E (xx, xx) values(xx,xx), (xx, xx), (xx, xx)
    /// </summary>
    /// <param name="entities"></param>
    /// <returns></returns>
    bool insert2(const QList<E>& entities);

    /// <summary>
    /// insert objects in batches
    /// use 'exec'，the values list connect by string(warning: sql statement length limit)
    /// insert into E (xx, xx) values(xx,xx), (xx, xx), (xx, xx)
    /// update if a unique constraint exists, otherwise insert
    /// </summary>
    /// <param name="entities"></param>
    /// <returns></returns>
    bool insert2OrReplace(const QList<E>& entities) {
        insertOrRp = true;
        return insert2(entities);
    }

private:
    bool buildInsertBySetSqlStatement();
    QString buildInsertObjectSqlStatement();
    QString buildInsertObjectsSqlStatement();
    QString buildInsertObjects2SqlStatement(int valueSize);

private:
    bool insertOrRp = false;

    BASE_QUERY_CONSTRUCTOR_DECLARE_W(Insert)
};

template<typename E>
inline bool Insert<E>::insert() {
    bool operateBatch = buildInsertBySetSqlStatement();
    bool execSuccess = false;
    if (operateBatch) {
        execBatch([&] (QSqlQuery&) {
            execSuccess = true;
        });
    } else {
        exec([&] (QSqlQuery&) {
            execSuccess = true;
        });
    }
    return execSuccess;
}

template<typename E>
inline bool Insert<E>::insert(E& entity) {
    auto sqlstatement = buildInsertObjectSqlStatement();
    typename E::Tool tool;
    auto values = tool.getValueWithoutAutoIncrement(entity);
    setSqlQueryStatement(sqlstatement, values);
    bool execSuccess = false;
    exec([&](QSqlQuery& query) {
        execSuccess = true;
        tool.bindAutoIncrementId(entity, query.lastInsertId());
    });
    return execSuccess;
}

template<typename E>
inline bool Insert<E>::insert(const QList<E>& entities) {
    auto sqlstatement = buildInsertObjectsSqlStatement();
    typename E::Info info;
    typename E::Tool tool;
    QList<QVariantList> values;
    for (int i = 0; i < info.fieldSize(); i++) {
        values << QVariantList();
    }
    int usedValueSize = 0;
    for (const auto& entity : entities) {
        auto v = tool.getValueWithoutAutoIncrement(entity);
        usedValueSize = v.size();
        for (int i = 0; i < v.size(); i++) {
            values[i] << v.at(i);
        }
    }
    QVariantList tagValues;
    for (int i = 0; i < usedValueSize; i++) {
        tagValues << QVariant(values.at(i));
    }
    setSqlQueryStatement(sqlstatement, tagValues);
    bool execSuccess = false;
    execBatch([&](QSqlQuery& query) {
        Q_UNUSED(query);
        execSuccess = true;
    });
    return execSuccess;
}

template<typename E>
inline bool Insert<E>::insert2(const QList<E>& entities) {
    int entitySize = entities.size();
    Q_ASSERT(entitySize != 0);
    auto sqlstatement = buildInsertObjects2SqlStatement(entitySize);
    QVariantList values;
    typename E::Tool tool;
    for (const auto& entity : entities) {
        auto v = tool.getValueWithoutAutoIncrement(entity);
        values.append(v);
    }
    setSqlQueryStatement(sqlstatement, values);
    bool execSuccess = false;
    exec([&](QSqlQuery& query) {
        Q_UNUSED(query);
        execSuccess = true;
    });
    return execSuccess;
}

template<typename E>
inline bool Insert<E>::buildInsertBySetSqlStatement() {
    builder->setCondition.connect();
    auto usedFieldName = builder->setCondition.getUsedFieldNames();
    QVariantList values = builder->setCondition.getValues();
    Q_ASSERT(!values.isEmpty());
    bool operateBatch = values.at(0).type() == QVariant::List;

    typename E::Info info;
    QString sql = "insert into %1 (";
    if (insertOrRp) {
        if (DbLoader::getConfig().isSqlite()) {
            sql = "insert or replace into %1 (";
        } else if (DbLoader::getConfig().isMysql()) {
            sql = "replace into %1 (";
        } else {
            Q_ASSERT_X(false, "insert", "not support!");
        }
    }
    sql = sql.arg(info.getTableName());

    bool hasPre = false;
    for (int i = 0; i < usedFieldName.size(); ) {
        auto field = usedFieldName.at(i);
        if (!info.isAutoIncrement(field.name)) {
            if (hasPre) {
                sql.append(",");
            }
            sql.append(field.name);
            hasPre = true;
            i++;
        } else {
            usedFieldName.removeAt(i);
            values.removeAt(i);
        }
    }
    sql.append(") values (");
    sql.append(QString("?,").repeated(values.size()));
    sql.chop(1);
    sql.append(")");
    setSqlQueryStatement(sql, values);
    return operateBatch;
}

template<typename E>
inline QString Insert<E>::buildInsertObjectSqlStatement() {
    typename E::Info info;
    
    QString sql = "insert into %1 (";
    if (insertOrRp) {
        if (DbLoader::getConfig().isSqlite()) {
            sql = "insert or replace into %1 (";
        } else if (DbLoader::getConfig().isMysql()) {
            sql = "replace into %1 (";
        } else {
            Q_ASSERT_X(false, "insert", "not support!");
        }
    }
    sql = sql.arg(info.getTableName());

    QStringList fields = info.getFieldsWithoutAutoIncrement();
    for (const auto& f : fields) {
        sql.append(f).append(",");
    }
    if (!fields.isEmpty()) {
        sql.chop(1);
    }
    sql.append(") values (");
    sql.append(QString("?,").repeated(fields.size()));
    sql.chop(1);
    sql.append(")");
    return sql;
}

template<typename E>
inline QString Insert<E>::buildInsertObjectsSqlStatement() {
    return buildInsertObjectSqlStatement();
}

template<typename E>
inline QString Insert<E>::buildInsertObjects2SqlStatement(int valueSize) {
    typename E::Info info;

    QString sql = "insert into %1 (";
    if (insertOrRp) {
        if (DbLoader::getConfig().isSqlite()) {
            sql = "insert or replace into %1 (";
        } else if (DbLoader::getConfig().isMysql()) {
            sql = "replace into %1 (";
        } else {
            Q_ASSERT_X(false, "insert", "not support!");
        }
    }
    sql = sql.arg(info.getTableName());

    QStringList fields = info.getFieldsWithoutAutoIncrement();
    for (const auto& f : fields) {
        sql.append(f).append(",");
    }
    if (!fields.isEmpty()) {
        sql.chop(1);
    }
    sql.append(") values ");
    QString vstr = "(";
    vstr.append(QString("?,").repeated(fields.size()));
    vstr.chop(1);
    vstr.append("),");
    sql.append(vstr.repeated(valueSize));
    sql.chop(1);
    return sql;
}

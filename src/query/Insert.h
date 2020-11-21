#pragma once

#include "BaseQuery.h"

template<typename E>
class Insert : public BaseQuery {
public:
    /// <summary>
    /// 使用set条件插入一条记录，可批量插入
    /// </summary>
    /// <returns></returns>
    bool insert();

    /// <summary>
    /// 插入一个对象实例，插入成功后将id设置回对象
    /// </summary>
    /// <param name="entity"></param>
    /// <returns></returns>
    bool insert(E& entity);

    /// <summary>
    /// 批量插入对象，插入成功后将id设置回对象列表
    /// 使用execbatch插入
    /// </summary>
    /// <param name="entities"></param>
    /// <returns></returns>
    bool insert(QList<E>& entities);

    /// <summary>
    /// 批量插入对象方式2
    /// 使用exec方式插入，值列表使用values拼接（警告：sql语句长度限制）
    /// insert into E values(xx,xx), (xx, xx), (xx, xx)
    /// </summary>
    /// <param name="entities"></param>
    /// <returns></returns>
    bool insert2(QList<E>& entities);

private:
    bool buildInsertBySetSqlStatement();

};

template<typename E>
inline bool Insert<E>::insert() {
    bool operateBatch = buildInsertBySetSqlStatement();
    bool execSuccess = false;
    if (operateBatch) {
        execBatch([&] (const QSqlQuery&) {
            execSuccess = true;
        });
    } else {
        exec([&] (const QSqlQuery&) {
            execSuccess = true;
        });
    }
    return execSuccess;
}

template<typename E>
inline bool Insert<E>::insert(E& entity) {

}

template<typename E>
inline bool Insert<E>::insert(QList<E>& entities) {

}

template<typename E>
inline bool Insert<E>::insert2(QList<E>& entities) {

}

template<typename E>
inline bool Insert<E>::buildInsertBySetSqlStatement() {
    connector.connect("");
    QStringList usedFieldName = connector.getUsedFieldNames();
    QVariantList values = connector.getValues();
    Q_ASSERT(!values.isEmpty());
    bool operateBatch = values.at(0).type() == QMetaType::QVariantList;

    typename E::Info info;
    QString sql = "insert into %1 (";
    sql = sql.arg(info.getTableName());

    bool hasPre = false;
    for (int i = 0; i < usedFieldName.size(); ) {
        auto field = usedFieldName.at(i);
        if (!info.isAutoIncrement(field)) {
            if (hasPre) {
                sql.append(",");
            }
            sql.append(field);
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

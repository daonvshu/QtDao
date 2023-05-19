#include "query/insertimpl.h"

#include "config/configbuilder.h"
#include "dbexception.h"

QTDAO_BEGIN_NAMESPACE

bool InsertImpl::buildInsertBySetSqlStatement() {

    auto& sc = setConnector();
    sc.combine();

    auto usedFieldName = sc.getUsedFields();
    QVariantList values = sc.getValueList();
    Q_ASSERT(!values.isEmpty());

#if QT_VERSION_MAJOR >= 6
    bool operateBatch = values.at(0).typeId() == QMetaType::QVariantList;
#else
    bool operateBatch = values.at(0).type() == QVariant::List;
#endif

    auto sql = buildInsertPrefix();

    bool hasPre = false;
    for (int i = 0; i < usedFieldName.size(); ) {
        auto field = usedFieldName.at(i);
        if (!isAutoIncrement(field.name)) {
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

QString InsertImpl::buildInsertObjectSqlStatement() {

    auto sql = buildInsertPrefix();

    QStringList fields = getFieldsWithoutAutoIncrement();
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

QString InsertImpl::buildInsertObjectsSqlStatement() {
    return buildInsertObjectSqlStatement();
}

QString InsertImpl::buildInsertObjects2SqlStatement(int valueSize) {

    auto sql = buildInsertPrefix();

    QStringList fields = getFieldsWithoutAutoIncrement();
    for (const auto& f : fields) {
        sql.append(f).append(",");
    }
    if (!fields.isEmpty()) {
        sql.chop(1);
    }
    sql.append(") values ");
    QString str = "(";
    str.append(QString("?,").repeated(fields.size()));
    str.chop(1);
    str.append("),");
    sql.append(str.repeated(valueSize));
    sql.chop(1);
    return sql;
}

QString InsertImpl::buildInsertPrefix() {

    QString sql;

    switch (insertMode) {
        case InsertMode::INSERT_ONLY:
            sql = "insert into %1 (";
            break;
        case InsertMode::INSERT_OR_REPLACE:
        {
            if (globalConfig->isSqlite()) {
                sql = "insert or replace into %1 (";
            } else if (globalConfig->isMysql()) {
                sql = "replace into %1 (";
            } else {
                throw DaoException("sqlserver insert or replace unsupported!");
            }
        }
            break;
        case InsertMode::INSERT_OR_IGNORE:
        {
            if (globalConfig->isSqlite()) {
                sql = "insert or ignore into %1 (";
            } else if (globalConfig->isMysql()) {
                sql = "insert ignore into %1 (";
            } else {
                throw DaoException("sqlserver insert or ignore unsupported!");
            }
        }
            break;
    }
    return sql.arg(getTableName());
}


QTDAO_END_NAMESPACE
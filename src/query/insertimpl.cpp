#include "query/insertimpl.h"

#include "dao.h"
#include "dbexceptionhandler.h"

QTDAO_BEGIN_NAMESPACE

bool InsertImpl::buildInsertBySetSqlStatement() {

    builder->setCondition.connect();

    auto usedFieldName = builder->setCondition.getUsedFieldNames();
    QVariantList values = builder->setCondition.getValues();
    Q_ASSERT(!values.isEmpty());

    bool operateBatch = values.at(0).type() == QVariant::List;

    QString sql = "insert into %1 (";
    if (insertOrRp) {
        if (globalConfig->isSqlite()) {
            sql = "insert or replace into %1 (";
        } else if (globalConfig->isMysql()) {
            sql = "replace into %1 (";
        } else {
            throw DaoException(DbErrCode::SQL_EXEC_FAIL, "sqlserver insert or replace unsupported!");
        }
    }

    sql = sql.arg(getTableName());

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

    QString sql = "insert into %1 (";
    if (insertOrRp) {
        if (globalConfig->isSqlite()) {
            sql = "insert or replace into %1 (";
        } else if (globalConfig->isMysql()) {
            sql = "replace into %1 (";
        } else {
            throw DaoException(DbErrCode::SQL_EXEC_FAIL, "sqlserver insert or replace unsupported!");
        }
    }
    sql = sql.arg(getTableName());

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

    QString sql = "insert into %1 (";
    if (insertOrRp) {
        if (globalConfig->isSqlite()) {
            sql = "insert or replace into %1 (";
        } else if (globalConfig->isMysql()) {
            sql = "replace into %1 (";
        } else {
            throw DaoException(DbErrCode::SQL_EXEC_FAIL, "sqlserver insert or replace unsupported!");
        }
    }
    sql = sql.arg(getTableName());

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


QTDAO_END_NAMESPACE
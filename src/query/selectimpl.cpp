#include "query/selectimpl.h"

#include "config/configbuilder.h"
#include "dbexception.h"

QTDAO_BEGIN_NAMESPACE

void SelectImpl::buildFilterSqlStatement() {
    QString sql = "select %1%2 from %3";
    QVariantList values;

    if (topSize != 0 && globalConfig->isSqlServer()) {
        sql = sql.arg("top " + QString::number(topSize) + (topPercent ? " percent " : " "));
    } else {
        sql = sql.arg("");
    }
    sql = sql.arg(getBindColumns(values));

    auto& fs = fromBuildData();
    if (fs.statement.isEmpty()) {
        sql = sql.arg(getTableName());
    } else {
        if (fs.recursiveQuery) {
            sql = sql.arg(fs.asName);
            sql = fs.statement + sql;
            fs.values.append(values);
            fs.values.swap(values);
        } else {
            sql = sql.arg('(' + fs.statement + ") as " + fs.asName);
            values.append(fs.values);
        }
    }

    auto& fc = filterConnector();
    if (!fc.isEmpty()) {
        fc.connect();
        sql.append(" where ").append(fc.getConditionStr());
        values.append(fc.getValues());
    }

    auto& cc = constraintConnector();
    if (!cc.isEmpty()) {
        cc.connect();
        sql.append(" ").append(cc.getConditionStr());
        values.append(cc.getValues());
    }

    auto& uc = unionBuildData();
    if (!uc.statement.isEmpty()) {
        sql.append(uc.unionAll ? " union all " : " union ");
        sql.append(uc.statement);
        values.append(uc.values);
    }

    setSqlQueryStatement(sql, values);
}

QString SelectImpl::getBindColumns(QVariantList &values) {
    auto& cc = columnConnector();
    if (cc.isEmpty()) {
        return "*";
    }
    cc.connect();
    values << cc.getValues();
    return cc.getConditionStr();
}

QString SelectImpl::readExplainStatement() {
    buildFilterSqlStatement();
    return globalConfig->getClient()->translateSqlStatement(statement, values);
}

void SelectImpl::recordBind(const QSqlRecord& record, const EntityBinder& entityBinder) {
    for (int i = 0; i < record.count(); i++) {
        entityBinder(record.fieldName(i), record.value(i));
    }
}

void SelectImpl::uniqueExec(const EntityBinder& entityBinder) {
    buildFilterSqlStatement();

    auto query = exec();

    int resultSize = 0;
    while (query.next()) {
        recordBind(query.record(), entityBinder);
        resultSize++;
    }
    if (resultSize > 1) {
        throw DaoException("unique result size > 1, actual is " + QString::number(resultSize));
    }
}

void SelectImpl::oneExec(const EntityBinder& entityBinder) {
    buildFilterSqlStatement();

    auto query = exec();
    if (query.next()) {
        recordBind(query.record(), entityBinder);
    }
}

void SelectImpl::listExec(const std::function<void(const QSqlRecord &)> &recordHandler) {
    buildFilterSqlStatement();

    auto query = exec();
    while (query.next()) {
        recordHandler(query.record());
    }
}


QTDAO_END_NAMESPACE
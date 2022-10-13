#include "query/selectimpl.h"
#include "dao.h"
#include "dbexceptionhandler.h"

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
    if (builder->fromSelectStatement.isEmpty()) {
        sql = sql.arg(getTableName());
    } else {
        if (builder->recursiveQuery) {
            sql = sql.arg(builder->fromSelectAs);
            sql = builder->fromSelectStatement + sql;
            builder->fromSelectValues.append(values);
            builder->fromSelectValues.swap(values);
        } else {
            sql = sql.arg('(' + builder->fromSelectStatement + ") as " + builder->fromSelectAs);
            values.append(builder->fromSelectValues);
        }
    }

    if (!builder->filterCondition.isEmpty()) {
        builder->filterCondition.connect();
        sql.append(" where ").append(builder->filterCondition.getConditionStr());
        values.append(builder->filterCondition.getValues());
    }

    if (!builder->constraintCondition.isEmpty()) {
        builder->constraintCondition.connect();
        sql.append(" ").append(builder->constraintCondition.getConditionStr());
        values.append(builder->constraintCondition.getValues());
    }

    if (!builder->unionSelectStatement.isEmpty()) {
        sql.append(builder->unionAll ? " union all " : " union ");
        sql.append(builder->unionSelectStatement);
        values.append(builder->unionSelectValues);
    }

    setSqlQueryStatement(sql, values);
}

QString SelectImpl::getBindColumns(QVariantList &values) {
    if (builder->columnBind.isEmpty()) {
        return "*";
    }
    builder->columnBind.connect();
    values << builder->columnBind.getValues();
    return builder->columnBind.getConditionStr();
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
        throw DaoException(DbErrCode::SQL_EXEC_FAIL, "unique result size > 1, actual is " + QString::number(resultSize));
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
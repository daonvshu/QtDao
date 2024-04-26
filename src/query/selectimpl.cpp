#include "query/selectimpl.h"

#include "config/configbuilder.h"
#include "config/configmanager.h"

#include "dbexception.h"

QTDAO_BEGIN_NAMESPACE

void SelectImpl::buildFilterSqlStatement() {
    QString sql = "select %1%2 from %3";
    QVariantList values;

    auto config = ConfigManager::getConfig(getSessionId());
    if (topSize != 0 && config->isSqlServer()) {
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
        fc.combine();
        sql.append(" where ").append(fc.getConditionSegment());
        values.append(fc.getValueList());
    }

    auto& cc = constraintConnector();
    if (!cc.isEmpty()) {
        cc.combine();
        sql.append(" ").append(cc.getConditionSegment());
        values.append(cc.getValueList());
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
    cc.combine();
    values << cc.getValueList();
    return cc.getConditionSegment();
}

QString SelectImpl::readExplainStatement() {
    buildFilterSqlStatement();
    auto config = ConfigManager::getConfig(getSessionId());
    return config->getClient()->translateSqlStatement(statement, values);
}

void SelectImpl::recordBind(const QSqlRecord& record, const EntityBinder& entityBinder) {
    for (int i = 0; i < record.count(); i++) {
        entityBinder(record.fieldName(i), record.value(i));
    }
}

void SelectImpl::uniqueExec(const EntityBinder& entityBinder, const RecordBinder& recordHandler) {
    buildFilterSqlStatement();

    auto query = exec(getSessionId());

    int resultSize = 0;
    while (query.next()) {
        if (entityBinder) {
            recordBind(query.record(), entityBinder);
        }
        if (recordHandler) {
            recordHandler(query.record());
        }
        resultSize++;
    }
    if (resultSize > 1) {
        throw DaoException("unique result size > 1, actual is " + QString::number(resultSize), this->statement, this->values);
    }
}

void SelectImpl::oneExec(const EntityBinder& entityBinder, const RecordBinder& recordHandler) {
    buildFilterSqlStatement();

    auto query = exec(getSessionId());
    if (query.next()) {
        if (entityBinder) {
            recordBind(query.record(), entityBinder);
        }
        if (recordHandler) {
            recordHandler(query.record());
        }
    }
}

void SelectImpl::listExec(const RecordBinder& recordHandler) {
    buildFilterSqlStatement();

    auto query = exec(getSessionId());
    while (query.next()) {
        recordHandler(query.record());
    }
}


QTDAO_END_NAMESPACE
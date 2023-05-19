﻿#include "query/basequery.h"

#include "connectionpool.h"
#include "dbexception.h"

#include "config/configbuilder.h"

QTDAO_BEGIN_NAMESPACE

Q_LOGGING_CATEGORY(loggingDefault, "qtdao.query")

bool BaseQuery::useDefaultLogging = false;

QSqlQuery BaseQuery::queryPrimitive(const QString& statement, const QVariantList& values, LoggingCategoryPtr logging, bool debugFatalEnabled) {
    BaseQuery executor;
    executor.setSqlQueryStatement(statement, values);
    executor.setDebug(debugFatalEnabled, logging);

    if (!values.isEmpty()) {
#if QT_VERSION_MAJOR >= 6
        if (values.first().typeId() == QMetaType::QVariantList) {
#else
        if (values.first().type() == QVariant::List) {
#endif
            return executor.execBatch();
        }
    }
    return executor.exec();
}

void BaseQuery::executePrimitiveQuery(const QString &statement, QString databaseName, QString connectionName, const std::function<void(QSqlQuery&)>& resultReader) {
    if (databaseName.isEmpty()) {
        databaseName = globalConfig->mDatabaseName;
    }

    if (connectionName.isEmpty()) {
        connectionName = "temp_execute_query";
    }

    QSqlError lastErr;
    do {
        auto db = ConnectionPool::prepareConnect(connectionName, databaseName);
        if (!db.open()) {
            lastErr = db.lastError();
            break;
        }
        QSqlQuery query(statement, db);
        lastErr = query.lastError();
        if (resultReader) {
            if (lastErr.type() == QSqlError::NoError) {
                resultReader(query);
            }
        }
        db.close();
    } while (false);

    QSqlDatabase::removeDatabase(connectionName);

    if (lastErr.type() != QSqlError::NoError) {
        throw DaoException(lastErr);
    }
}

void BaseQuery::useDefaultLoggingIfNull(bool useDefault) {
    useDefaultLogging = useDefault;
}

void BaseQuery::setSqlQueryStatement(const QString& curStatement, const QVariantList& curValues) {
    this->statement = curStatement;
    this->values = curValues;
}

void BaseQuery::setDebug(bool fatalEnabled, LoggingCategoryPtr logging) {
    this->debugFatalEnabled = fatalEnabled;
    this->loggingCategoryPtr = logging;
}

QSqlQuery BaseQuery::exec() {
    bool prepareOk;
    auto query = getQuery(prepareOk, false);
    if (!prepareOk || !execByCheckEmptyValue(query, this)) {
        postError(query, debugFatalEnabled, !prepareOk);
    }
    return query;
}

QSqlQuery BaseQuery::execBatch() {
    bool prepareOk;
    auto query = getQuery(prepareOk, true);
    if (!prepareOk || !query.execBatch()) {
        postError(query, debugFatalEnabled, !prepareOk);
    }
    return query;
}

QSqlQuery BaseQuery::getQuery(bool& prepareOk, bool batchExecMode) {
    auto db = ConnectionPool::getConnection();
    QSqlQuery query(db);
    if (batchExecMode || !values.isEmpty()) {
        if (!query.prepare(statement)) {
            prepareOk = false;
            return query;
        }
        bindQueryValues(query);
    }
    if (loggingCategoryPtr == nullptr) {
        if (useDefaultLogging) {
            loggingCategoryPtr = loggingDefault;
        }
    }
    if (loggingCategoryPtr != nullptr) {
        if (loggingCategoryPtr().isDebugEnabled()) {
            printQueryLog(this, batchExecMode);
        }
    }

    prepareOk = true;
    return query;
}

void BaseQuery::bindQueryValues(QSqlQuery& query) {
    for (const auto& d : values) {
#if QT_VERSION_MAJOR >= 6
        query.addBindValue(d, d.typeId() == QMetaType::QByteArray ? QSql::Binary : QSql::In);
#else
        query.addBindValue(d, d.type() == QVariant::ByteArray ? QSql::Binary : QSql::In);
#endif
    }
}

void BaseQuery::postError(const QSqlQuery& lastQuery, bool fatalEnabled, bool prepareStatementFail) {
    auto lastErr = lastQuery.lastError();
    auto errText = lastErr.text();
    Q_UNUSED(errText)
    if (fatalEnabled) {
        fatalError(prepareStatementFail);
    }
    throw DaoException(lastErr);
}

void BaseQuery::fatalError(bool prepareError) {
#ifdef QT_DEBUG
    if (prepareError) {
        qFatal("get connection prepare fail!");
    } else {
        qFatal("execute sql statement fail!");
    }
#else
    Q_UNUSED(prepareError)
#endif
}

bool BaseQuery::execByCheckEmptyValue(QSqlQuery& query, const BaseQuery* executor) {
    if (!executor->values.isEmpty()) {
        return query.exec();
    }
    return query.exec(executor->statement);
}

QList<SqliteExplainInfo> BaseQuery::ExplainTool<SqliteExplainInfo>::toExplain(const QString& statement) {
    Q_ASSERT_X(globalConfig->isSqlite(), "ExplainTool<SqliteExplainInfo>", "need config sqlite");
    QSqlQuery query = BaseQuery::queryPrimitive("explain " + statement);
    QList<SqliteExplainInfo> result;
    while (query.next()) {
        SqliteExplainInfo info;
        info.addr = query.value("addr").toLongLong();
        info.opcode = query.value("opcode").toString();
        info.p1 = query.value("p1").toLongLong();
        info.p2 = query.value("p2").toLongLong();
        info.p3 = query.value("p3").toLongLong();
        info.p4 = query.value("p4").toString();
        info.p5 = query.value("p5").toString();
        info.comment = query.value("comment").toString();
        result << info;
    }
    return result;
}

QList<SqliteExplainQueryPlanInfo> BaseQuery::ExplainTool<SqliteExplainQueryPlanInfo>::toExplain(
    const QString& statement) {
    Q_ASSERT_X(globalConfig->isSqlite(), "ExplainTool<SqliteExplainQueryPlanInfo>", "need config sqlite");
    QSqlQuery query = BaseQuery::queryPrimitive("explain query plan " + statement);
    QList<SqliteExplainQueryPlanInfo> result;
    while (query.next()) {
        SqliteExplainQueryPlanInfo info;
        QSqlRecord record = query.record();
        for (int i = 0; i < record.count(); i++) {
            info.data << qMakePair(record.fieldName(i), record.value(i));
        }
        result << info;
    }
    return result;
}

QList<MysqlExplainInfo> BaseQuery::ExplainTool<MysqlExplainInfo>::toExplain(const QString& statement) {
    Q_ASSERT_X(globalConfig->isMysql(), "ExplainTool<MysqlExplainInfo>", "need config mysql");
    QSqlQuery query = BaseQuery::queryPrimitive("explain " + statement);
    QList<MysqlExplainInfo> result;
    while (query.next()) {
        MysqlExplainInfo info;
        info.id = query.value("id").toULongLong();
        info.selectType = query.value("select_type").toString();
        info.table = query.value("table").toString();
        info.partitions = query.value("partitions").toString();
        info.type = query.value("type").toString();
        info.possibleKeys = query.value("possible_keys").toString();
        info.key = query.value("key").toString();
        info.keyLen = query.value("key_len").toString();
        info.ref = query.value("ref").toString();
        info.rows = query.value("rows").toULongLong();
        info.filtered = query.value("filtered").toDouble();
        info.extra = query.value("Extra").toString();
        result << info;
    }
    return result;
}

//TODO
QList<SqlServerExplainInfo> BaseQuery::ExplainTool<SqlServerExplainInfo>::toExplain(const QString& statement) {
    Q_ASSERT_X(globalConfig->isSqlServer(), "ExplainTool<SqlServerExplainInfo>", "need config sqlserver");
    QList<SqlServerExplainInfo> result;
    try {
        QSqlQuery query = BaseQuery::queryPrimitive("SET STATISTICS PROFILE ON;" + statement + ";SET STATISTICS PROFILE OFF");
        if (query.nextResult()) {
            while (query.next()) {
                SqlServerExplainInfo info;
                info.Rows = query.value("Rows").toLongLong();
                info.Executes = query.value("Executes").toLongLong();
                info.StmtText = query.value("StmtText").toString();
                info.StmtId = query.value("StmtId").toInt();
                info.NodeId = query.value("NodeId").toInt();
                info.Parent = query.value("Parent").toInt();
                info.PhysicalOp = query.value("PhysicalOp").toString();
                info.LogicalOp = query.value("LogicalOp").toString();
                info.Argument = query.value("Argument").toString();
                info.DefinedValues = query.value("DefinedValues").toString();
                info.EstimateRows = query.value("EstimateRows").toDouble();
                info.EstimateIO = query.value("EstimateIO").toDouble();
                info.EstimateCPU = query.value("EstimateCPU").toDouble();
                info.AvgRowSize = query.value("AvgRowSize").toInt();
                info.TotalSubtreeCost = query.value("TotalSubtreeCost").toDouble();
                info.OutputList = query.value("OutputList").toString();
                info.Warnings = query.value("Warnings").toString();
                info.Type = query.value("Type").toString();
                info.Parallel = query.value("Parallel").toUInt();
                info.EstimateExecutions = query.value("EstimateExecutions").toDouble();
                result << info;
            }
        }
    } catch (DaoException&) {
    }
    return result;
}

QTDAO_END_NAMESPACE
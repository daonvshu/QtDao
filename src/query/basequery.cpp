#include "query/basequery.h"

#include "connectionpool.h"
#include "dbexceptionhandler.h"
#include "dao.h"

#include <iostream>
#include <QThread>

QTDAO_BEGIN_NAMESPACE

DbErrCode::Code BaseQuery::exceptionLastErr = DbErrCode::ERR_NOT_SET;

BaseQuery::BaseQuery(bool throwable, BaseQueryBuilder* builder)
    : builder(builder)
{
    if (builder != nullptr) {
        this->builder = new BaseQueryBuilder(*builder);
    }
}

BaseQuery::BaseQuery(const BaseQuery& other) {
    this->statement = other.statement;
    this->values = other.values;
    this->builder = new BaseQueryBuilder(*other.builder);
}

BaseQuery::~BaseQuery() {
    if (builder != nullptr) {
        delete builder;
        builder = nullptr;
    }
}

QSqlQuery BaseQuery::exec() {
    bool prepareOk;
    auto query = getQuery(prepareOk, true);
    if (!prepareOk || !execByCheckEmptyValue(query, this)) {
        throw DaoException(getLastErrCode(), query.lastError().text());
    }
    return query;
}

QSqlQuery BaseQuery::execBatch() {
    bool prepareOk;
    auto query = getQuery(prepareOk);
    if (!prepareOk || !query.execBatch()) {
        throw DaoException(getLastErrCode(), query.lastError().text());
    }
    return query;
}

void BaseQuery::queryPrimitive(const QString& statement,
    const std::function<void(QSqlQuery& query)>& callback,
    const std::function<void(QString)>& failCallback
) {
    queryPrimitive(statement, QVariantList(), callback, failCallback);
}

void BaseQuery::queryPrimitive(const QString& statement, 
    const QVariantList& values, 
    const std::function<void(QSqlQuery& query)>& callback,
    const std::function<void(QString)>& failCallback
) {
    BaseQuery executor;
    executor.setSqlQueryStatement(statement, values);
    bool prepareOk;
    auto query = executor.getQuery(prepareOk, true);
    if (prepareOk && execByCheckEmptyValue(query, &executor)) {
        if (callback) {
            callback(query);
        }
    } else {
        auto lastErr = query.lastError().text();
        if (failCallback) {
            failCallback(lastErr);
        } else {
            throw DaoException(getLastErrCode(), query.lastError().text());
        }
    }
}

QSqlQuery BaseQuery::queryPrimitiveThrowable(
    const QString& statement
) {
    return queryPrimitiveThrowable(statement, QVariantList());
}

QSqlQuery BaseQuery::queryPrimitiveThrowable(
    const QString& statement, 
    const QVariantList& values
) {
    BaseQuery executor;
    executor.setSqlQueryStatement(statement, values);
    bool prepareOk;
    auto query = executor.getQuery(prepareOk, true);
    if (prepareOk && execByCheckEmptyValue(query, &executor)) {
        return query;
    } else {
        throw DaoException(getLastErrCode(), query.lastError().text());
    }
}

void BaseQuery::setErrIfQueryFail(DbErrCode::Code code) {
    exceptionLastErr = code;
}

void BaseQuery::setSqlQueryStatement(const QString& statement, const QVariantList& values) {
    this->statement = statement;
    this->values = values;
}

QSqlQuery BaseQuery::getQuery(bool& prepareOk, bool skipEmptyValue) {
    auto db = ConnectionPool::getConnection();
    QSqlQuery query(db);
    if (!skipEmptyValue || !values.isEmpty()) {
        if (!query.prepare(statement)) {
            prepareOk = false;
            return query;
        }
        bindQueryValues(query);
    }
    if (getQueryLogPrinter()) {
        getQueryLogPrinter()(statement, values);
    }
    prepareOk = true;
    return query;
}

void BaseQuery::bindQueryValues(QSqlQuery& query) {
    for (const auto& d : values) {
        query.addBindValue(d, d.type() == QVariant::ByteArray ? QSql::Binary : QSql::In);
    }
}

bool BaseQuery::execByCheckEmptyValue(QSqlQuery& query, const BaseQuery* executor) {
    if (!executor->values.isEmpty()) {
        return query.exec();
    }
    return query.exec(executor->statement);
}

DbErrCode::Code BaseQuery::getLastErrCode() {
    auto errcode = exceptionLastErr;
    if (errcode == DbErrCode::ERR_NOT_SET) {
        errcode = DbErrCode::SQL_EXEC_FAIL;
    }
    exceptionLastErr = DbErrCode::ERR_NOT_SET;
    return errcode;
}

QList<SqliteExplainInfo> BaseQuery::ExplainTool<SqliteExplainInfo>::toExplain(const QString& statement) {
    Q_ASSERT_X(globalConfig->isSqlite(), "ExplainTool<SqliteExplainInfo>", "need config sqlite");
    QSqlQuery query = BaseQuery::queryPrimitiveThrowable("explain " + statement);
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
    QSqlQuery query = BaseQuery::queryPrimitiveThrowable("explain query plan " + statement);
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
    QSqlQuery query = BaseQuery::queryPrimitiveThrowable("explain " + statement);
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
        QSqlQuery query = BaseQuery::queryPrimitiveThrowable("SET STATISTICS PROFILE ON;" + statement + ";SET STATISTICS PROFILE OFF");
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
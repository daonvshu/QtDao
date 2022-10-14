#include "dbexceptionhandler.h"
#include "dao.h"

QTDAO_BEGIN_NAMESPACE

QScopedPointer<ConfigBuilder> globalConfig;

static QueryLogPrinter queryLogPrinter = nullptr;
void daoSetQueryLogPrinter(QueryLogPrinter printer) {
    queryLogPrinter = printer;
}

QueryLogPrinter getQueryLogPrinter() {
    return queryLogPrinter;
}

void transcation() {
    if (globalConfig->isSqlServer()) {
        BaseQuery::queryPrimitiveThrowable("begin tran");
    } else {
        BaseQuery::queryPrimitiveThrowable("begin");
    }
}

void commit() {
    BaseQuery::queryPrimitiveThrowable("commit");
}

void transcation_save(const QString& savePoint) {
    if (globalConfig->isSqlServer()) {
        BaseQuery::queryPrimitiveThrowable(QString("save tran %1").arg(savePoint));
    } else {
        BaseQuery::queryPrimitiveThrowable(QString("savepoint %1").arg(savePoint));
    }
}

void rollback(const QString& savePoint) {
    if (globalConfig->isSqlServer()) {
        BaseQuery::queryPrimitiveThrowable(
            savePoint.isEmpty() ? QString("rollback tran") : QString("rollback tran %1").arg(savePoint)
        );
    } else {
        BaseQuery::queryPrimitiveThrowable(
            savePoint.isEmpty() ? QString("rollback") : QString("rollback to %1").arg(savePoint)
        );
    }
}

QTDAO_END_NAMESPACE
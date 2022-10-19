#include "dbexception.h"
#include "dao.h"

QTDAO_BEGIN_NAMESPACE

QScopedPointer<ConfigBuilder> globalConfig;

void transcation() {
    if (globalConfig->isSqlServer()) {
        BaseQuery::queryPrimitive("begin tran");
    } else {
        BaseQuery::queryPrimitive("begin");
    }
}

void commit() {
    BaseQuery::queryPrimitive("commit");
}

void transcation_save(const QString& savePoint) {
    if (globalConfig->isSqlServer()) {
        BaseQuery::queryPrimitive(QString("save tran %1").arg(savePoint));
    } else {
        BaseQuery::queryPrimitive(QString("savepoint %1").arg(savePoint));
    }
}

void rollback(const QString& savePoint) {
    if (globalConfig->isSqlServer()) {
        BaseQuery::queryPrimitive(
            savePoint.isEmpty() ? QString("rollback tran") : QString("rollback tran %1").arg(savePoint)
        );
    } else {
        BaseQuery::queryPrimitive(
            savePoint.isEmpty() ? QString("rollback") : QString("rollback to %1").arg(savePoint)
        );
    }
}

QTDAO_END_NAMESPACE
#pragma once

#include "connectionpool.h"
#include "dbloader.h"

#include "builder/insertbuilder.h"
#include "builder/selectbuilder.h"
#include "builder/countbuilder.h"
#include "builder/updatebuilder.h"
#include "builder/deletebuilder.h"
#include "builder/joinbuilder.h"
#include "builder/recursivequerybuilder.h"
#include "builder/insertintoselectbuilder.h"

#include "condition/conditionoperator.h"

#include <QtSql/QSqlDriver>

QTDAO_BEGIN_NAMESPACE

template<typename T>
static InsertBuilder<T> _insert() {
    return InsertBuilder<T>();
}

template<typename T>
static InsertIntoSelectBuilder<T> _insertIntoSelect() {
    return InsertIntoSelectBuilder<T>();
}

template<typename T>
static SelectBuilder<T> _select() {
    return SelectBuilder<T>();
}

template<typename T>
static QList<T> _selectAll() {
    return SelectBuilder<T>().build().list();
}

template<typename T>
static CountBuilder<T> _count() {
    return CountBuilder<T>();
}

template<typename T>
static UpdateBuilder<T> _update() {
    return UpdateBuilder<T>();
}

template<typename T>
static DeleteBuilder<T> _delete() {
    return DeleteBuilder<T>();
}

template<typename... T>
static JoinBuilder<T...> _join() {
    return JoinBuilder<T...>();
}

static RecursiveQueryBuilder _recursive(bool unionAll = false) {
    Q_ASSERT_X(!DbLoader::getConfig().isMysql(), "RecursiveQuery", "not support mysql recursive query!");
    return RecursiveQueryBuilder(unionAll);
}

template<typename E>
static void _truncate() {
    DbLoader::getClient().truncateTable(E::Info::getTableName());
}

static void transcation() {
    BaseQuery::sqliteLockControl.trancationStart();
    if (DbLoader::getConfig().isSqlServer()) {
        BaseQuery::queryPrimitiveThrowable("begin tran");
    } else {
        BaseQuery::queryPrimitiveThrowable("begin");
    }
}

static void commit() {
    BaseQuery::sqliteLockControl.trancationPrepareEnd();
    BaseQuery::queryPrimitiveThrowable("commit");
    BaseQuery::sqliteLockControl.trancationEnd();
}

static void transcation_save(const QString& savePoint) {
    if (DbLoader::getConfig().isSqlServer()) {
        BaseQuery::queryPrimitiveThrowable(QString("save tran %1").arg(savePoint));
    } else {
        BaseQuery::queryPrimitiveThrowable(QString("savepoint %1").arg(savePoint));
    }
}

static void rollback(const QString& savePoint = QString()) {
    if (savePoint.isEmpty()) {
        BaseQuery::sqliteLockControl.trancationPrepareEnd();
    }
    if (DbLoader::getConfig().isSqlServer()) {
        BaseQuery::queryPrimitiveThrowable(
            savePoint.isEmpty() ? QString("rollback tran") : QString("rollback tran %1").arg(savePoint)
        );
    } else {
        BaseQuery::queryPrimitiveThrowable(
            savePoint.isEmpty() ? QString("rollback") : QString("rollback to %1").arg(savePoint)
        );
    }
    if (savePoint.isEmpty()) {
        BaseQuery::sqliteLockControl.trancationEnd();
    }
}

static void sqlWriteSync(bool enable = true) {
    BaseQuery::sqliteLockControl.enableSqliteWriteSync(enable);
}

template<typename E>
class self : public E {
public:
    struct Info : E::Info {
        enum {
            Attach = E::Info::Attach + 1
        };

        static QString getSourceName() {
            return E::Info::getSourceName();
        }

        static QString getTableName() {
            return getSourceName() + "_" + QString::number(Attach);
        }
    };

    class Fields : public E::Fields {
    public:
        Fields() {
            this->reset(Info::getTableName());
        }
    };
};

class LocalQuery {
public:
    ~LocalQuery() {
        ConnectionPool::closeConnection();
    }
};

QTDAO_END_NAMESPACE

#pragma once

#include "connectionpool.h"
#include "dbexception.h"

#include "builder/insertbuilder.h"
#include "builder/selectbuilder.h"
#include "builder/countbuilder.h"
#include "builder/updatebuilder.h"
#include "builder/deletebuilder.h"
#include "builder/joinbuilder.h"
#include "builder/recursivequerybuilder.h"
#include "builder/insertintoselectbuilder.h"
#include "builder/upsertbuilder.h"

#include "condition/conditionoperator.h"

#include "config/configsqlite.h"
#include "config/configmysql.h"
#include "config/configsqlserver.h"

#include <QtSql/QSqlDriver>

QTDAO_BEGIN_NAMESPACE

template<typename T>
struct TypeIdentify {
    typedef T type;
};

static ConfigSqliteBuilder _config(TypeIdentify<ConfigSqliteBuilder>) {
    return ConfigSqliteBuilder();
}

static ConfigMysqlBuilder _config(TypeIdentify<ConfigMysqlBuilder>) {
    return ConfigMysqlBuilder();
}

static ConfigSqlServerBuilder _config(TypeIdentify<ConfigSqlServerBuilder>) {
    return ConfigSqlServerBuilder();
}

template<typename T>
static T _config() {
    return _config(TypeIdentify<T>());
}

template<typename T>
static InsertBuilder<T> _insert() {
    return InsertBuilder<T>();
}

template<typename T>
static InsertIntoSelectBuilder<T> _insertIntoSelect() {
    return InsertIntoSelectBuilder<T>();
}

template<typename T>
static UpsertBuilder<T> _insertOrUpdate() {
    return UpsertBuilder<T>();
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
    Q_ASSERT_X(!globalConfig->isMysql(), "RecursiveQuery", "mysql recursive query unsupported!");
    return RecursiveQueryBuilder(unionAll);
}

template<typename E>
static void _truncate() {
    globalConfig->getClient()->truncateTable(E::Info::getTableName());
}

extern void transcation();

extern void commit();

extern void transcation_save(const QString& savePoint);

extern void rollback(const QString& savePoint = QString());

extern void loggingUseDefault(bool useDefault = true);

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

QTDAO_END_NAMESPACE

#define SCOPE_CONNECTION \
auto connectionCleanUp = qScopeGuard([] { ConnectionPool::closeConnection(); }); \
Q_UNUSED(connectionCleanUp)

#pragma once

#include "connectionpool.h"
#include "dbexception.h"
#include "configselector.h"

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
#include "config/configmanager.h"

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
static InsertBuilder<T> _insert(qint64 sessionId = -1) {
    return InsertBuilder<T>(sessionId);
}

template<typename T>
static InsertIntoSelectBuilder<T> _insertIntoSelect(qint64 sessionId = -1) {
    return InsertIntoSelectBuilder<T>(sessionId);
}

template<typename T>
static UpsertBuilder<T> _insertOrUpdate(qint64 sessionId = -1) {
    return UpsertBuilder<T>(sessionId);
}

template<typename T>
static SelectBuilder<T> _select(qint64 sessionId = -1) {
    return SelectBuilder<T>(sessionId);
}

template<typename T>
static QList<T> _selectAll(qint64 sessionId = -1) {
    return SelectBuilder<T>(sessionId).build().list();
}

template<typename T>
static CountBuilder<T> _count(qint64 sessionId = -1) {
    return CountBuilder<T>(sessionId);
}

template<typename T>
static UpdateBuilder<T> _update(qint64 sessionId = -1) {
    return UpdateBuilder<T>(sessionId);
}

template<typename T>
static DeleteBuilder<T> _delete(qint64 sessionId = -1) {
    return DeleteBuilder<T>(sessionId);
}

template<typename... T>
static JoinBuilder<T...> _join(qint64 sessionId = -1) {
    return JoinBuilder<T...>(sessionId);
}

extern RecursiveQueryBuilder _recursive(bool unionAll = false, qint64 sessionId = -1);

template<typename E>
static void _truncate(qint64 sessionId = -1) {
    _truncate(E::Info::getTableName(), sessionId);
}

extern void _truncate(const QString& tbName, qint64 sessionId = -1);

extern void transaction(qint64 sessionId = -1, LoggingCategoryPtr logging = nullptr);

extern void commit(qint64 sessionId = -1, LoggingCategoryPtr logging = nullptr);

extern void transaction_save(const QString& savePoint, qint64 sessionId = -1, LoggingCategoryPtr logging = nullptr);

extern void rollback(const QString& savePoint = QString(), qint64 sessionId = -1, LoggingCategoryPtr logging = nullptr);

extern void loggingUseDefault(bool useDefault = true);

extern void _beginSession(qint64 sessionId);

extern void _endSession();

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

template<typename E>
class UpsertExcluded : public E {
public:
    struct Info : E::Info {
        static QString getSourceName() {
            return getTableName();
        }

        static QString getTableName() {
            return "excluded";
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
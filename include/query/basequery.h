#pragma once

#include "../global.h"

#include <qvariant.h>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlError>

#include <functional>
#include <qmutex.h>
#include <qwaitcondition.h>

#include "../builder/basequerybuilder.h"

#include "../query/explaininfo.h"

#include "../dberrcode.h"

QTDAO_BEGIN_NAMESPACE

class BaseQuery {
public:
    explicit BaseQuery(bool throwable = false, BaseQueryBuilder* builder = nullptr);

    BaseQuery(const BaseQuery& other);
    ~BaseQuery();

    static void queryPrimitive(
        const QString& statement, 
        const std::function<void(QSqlQuery& query)>& callback = nullptr,
        const std::function<void(QString)>& failCallback = nullptr
    );

    static void queryPrimitive(
        const QString& statement,
        const QVariantList& values,
        const std::function<void(QSqlQuery& query)>& callback = nullptr,
        const std::function<void(QString)>& failCallback = nullptr
    );

    static QSqlQuery queryPrimitiveThrowable(
        const QString& statement
    );

    static QSqlQuery queryPrimitiveThrowable(
        const QString& statement,
        const QVariantList& values
    );

    static void setErrIfQueryFail(DbErrCode::Code code);

protected:
    void setSqlQueryStatement(const QString& statement, const QVariantList& values);

    QSqlQuery exec();
    QSqlQuery execBatch();

protected:
    QString statement;
    QVariantList values;
    BaseQueryBuilder* builder;

    friend class BaseQueryBuilder;

    static DbErrCode::Code exceptionLastErr;

    friend void transcation();
    friend void commit();
    friend void rollback(const QString&);
    friend void sqlWriteSync(bool);

private:
    QSqlQuery getQuery(bool& prepareOk, bool skipEmptyValue = false);
    void bindQueryValues(QSqlQuery& query);
    static bool execByCheckEmptyValue(QSqlQuery& query, const BaseQuery* executor);
    static DbErrCode::Code getLastErrCode();

protected:
    template<typename I>
    struct ExplainTool {
        enum {
            Valid = 0
        };
    };
};

template <>
struct BaseQuery::ExplainTool<SqliteExplainInfo> {
    enum {
        Valid = 1
    };

    static QList<SqliteExplainInfo> toExplain(const QString& statement);
};

template <>
struct BaseQuery::ExplainTool<SqliteExplainQueryPlanInfo> {
    enum {
        Valid = 1
    };

    static QList<SqliteExplainQueryPlanInfo> toExplain(const QString& statement);
};

template <>
struct BaseQuery::ExplainTool<MysqlExplainInfo> {
    enum {
        Valid = 1
    };

    static QList<MysqlExplainInfo> toExplain(const QString& statement);
};

template<>
struct BaseQuery::ExplainTool<SqlServerExplainInfo> {
    enum {
        Valid = 1
    };

    static QList<SqlServerExplainInfo> toExplain(const QString& statement);
};

QTDAO_END_NAMESPACE
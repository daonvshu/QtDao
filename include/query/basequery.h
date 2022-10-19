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

#include "../utils/logging.h"

QTDAO_BEGIN_NAMESPACE

class BaseQuery {
public:
    explicit BaseQuery(bool fatalEnabled = true, BaseQueryBuilder* builder = nullptr, LoggingCategoryPtr logging = nullptr);

    BaseQuery(const BaseQuery& other);
    ~BaseQuery();

    static QSqlQuery queryPrimitive(const QString& statement, const QVariantList& values = QVariantList(), LoggingCategoryPtr logging = nullptr, bool debugFatalEnabled = true);

protected:
    void setSqlQueryStatement(const QString& curStatement, const QVariantList& curValues);

    QSqlQuery exec();
    QSqlQuery execBatch();

protected:
    QString statement;
    QVariantList values;
    BaseQueryBuilder* builder;

    bool debugFatalEnabled;

    friend class BaseQueryBuilder;

    friend void transcation();
    friend void commit();
    friend void rollback(const QString&);
    friend void printQueryLog(BaseQuery* query, bool batchExecMode);

private:
    LoggingCategoryPtr loggingCategoryPtr;

private:
    QSqlQuery getQuery(bool& prepareOk, bool skipEmptyValue = false);
    void bindQueryValues(QSqlQuery& query);

    static void postError(const QSqlQuery& lastQuery, bool fatalEnabled, bool prepareStatementFail);
    static void fatalError(bool prepareError);
    static bool execByCheckEmptyValue(QSqlQuery& query, const BaseQuery* executor);

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
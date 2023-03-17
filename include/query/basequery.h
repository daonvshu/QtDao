#pragma once

#include "../global.h"

#include <qvariant.h>
#include <qsqlquery.h>
#include <qsqlrecord.h>

#include "../query/explaininfo.h"
#include "../builder/option/debugbuilder.h"
#include "../utils/logging.h"

QTDAO_BEGIN_NAMESPACE

class BaseQuery {
public:
    /**
     * execute sql statement directly
     * @param statement sql query statement
     * @param values bound the values to prepared statement
     * @param logging current logging category function ptr
     * @param debugFatalEnabled use qFatal in debug mode
     * @return
     */
    static QSqlQuery queryPrimitive(const QString& statement, const QVariantList& values = QVariantList(), LoggingCategoryPtr logging = nullptr, bool debugFatalEnabled = true);

    /**
     * use default logging category if not set 'LoggingCategoryPtr'
     * @param useDefault use default logging
     */
    static void useDefaultLoggingIfNull(bool useDefault);

protected:
    /**
     * add current query sql statement and prepared values
     * @param curStatement sql query statement
     * @param curValues prepared values
     */
    void setSqlQueryStatement(const QString& curStatement, const QVariantList& curValues);

    /**
     * set fatal enable or logging category
     * @param fatalEnabled
     * @param logging
     */
    void setDebug(bool fatalEnabled, LoggingCategoryPtr logging);

    template<typename T>
    void setDebug(DebugBuilder<T>* builder) {
        setDebug(builder->setFatalEnabled, builder->loggingCategoryPtr);
    }

    /**
     * execute prepared sql query
     * @return
     */
    QSqlQuery exec();

    /**
     * execute prepared sql query in a batch
     * @return
     */
    QSqlQuery execBatch();

protected:
    //sql statement
    QString statement;
    //prepare bound values
    QVariantList values;

private:
    //enable fatal error
    bool debugFatalEnabled;
    //current query logging category
    LoggingCategoryPtr loggingCategoryPtr;

    //use default category if not set
    static bool useDefaultLogging;

    //print formatted query error message
    friend void printQueryLog(BaseQuery* query, bool batchExecMode);

private:
    /**
     * get a sql connection from connection pool
     * @param prepareOk set prepare statement result
     * @param batchExecMode current execute in batch mode
     * @return opened query connection
     */
    QSqlQuery getQuery(bool& prepareOk, bool batchExecMode);

    /**
     * bound values to prepared query
     * @param query
     */
    void bindQueryValues(QSqlQuery& query);

    /**
     * get last query error and throw exception or fatal error
     * @param lastQuery current execute query
     * @param fatalEnabled post error use fatal error
     * @param prepareStatementFail current is prepare statement error
     */
    static void postError(const QSqlQuery& lastQuery, bool fatalEnabled, bool prepareStatementFail);

    /**
     * post a fatal error if current is debug mode
     * @param prepareError
     */
    static void fatalError(bool prepareError);

    /**
     * check the values to execute sql query, if values is not empty, only call 'exec()',
     * otherwise call 'exec(statement)'
     * @param query opened sql connection query
     * @param executor
     * @return execute result
     */
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
#pragma once

#include "../global.h"

#include <qvariant.h>
#include <qsqlquery.h>
#include <qsqlrecord.h>
#include <functional>

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
     * @param sessionId session for different database config
     * @param logging current logging category function ptr
     * @param debugFatalEnabled use qFatal in debug mode
     * @return
     */
    static QSqlQuery queryPrimitive(const QString& statement,
                                    const QVariantList& values = QVariantList(),
                                    qint64 sessionId = -1,
                                    LoggingCategoryPtr logging = nullptr,
                                    bool debugFatalEnabled = true);

    /**
     * execute sql statement directly, and use temporary database connection without connection pool,
     * throw 'DaoException' when execute fail
     * @param statement sql query statement
     * @param sessionId session for different database config
     * @param databaseName database name default to config database
     * @param connectionName connection name
     * @param resultReader query result reader
     */
    static void executePrimitiveQuery(const QString& statement,
                                      qint64 sessionId = -1,
                                      const QString& databaseName = QString(),
                                      QString connectionName = QString(),
                                      const std::function<void(QSqlQuery&)>& resultReader = nullptr);

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
    void setDebug(const DebugBuilder<T>& builder) {
        setDebug(builder.setFatalEnabled, builder.loggingCategoryPtr);
    }

    /**
     * execute prepared sql query
     * @param sessionId session for different database config
     * @return
     */
    QSqlQuery exec(qint64 sessionId);

    /**
     * execute prepared sql query in a batch
     * @param sessionId session for different database config
     * @return
     */
    QSqlQuery execBatch(qint64 sessionId);

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
     * @param sessionId session for different database config
     * @param prepareOk set prepare statement result
     * @param batchExecMode current execute in batch mode
     * @return opened query connection
     */
    QSqlQuery getQuery(qint64 sessionId, bool& prepareOk, bool batchExecMode);

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

    static QList<SqliteExplainInfo> toExplain(const QString& statement, qint64 sessionId = -1);
};

template <>
struct BaseQuery::ExplainTool<SqliteExplainQueryPlanInfo> {
    enum {
        Valid = 1
    };

    static QList<SqliteExplainQueryPlanInfo> toExplain(const QString& statement, qint64 sessionId = -1);
};

template <>
struct BaseQuery::ExplainTool<MysqlExplainInfo> {
    enum {
        Valid = 1
    };

    static QList<MysqlExplainInfo> toExplain(const QString& statement, qint64 sessionId = -1);
};

template<>
struct BaseQuery::ExplainTool<SqlServerExplainInfo> {
    enum {
        Valid = 1
    };

    static QList<SqlServerExplainInfo> toExplain(const QString& statement, qint64 sessionId = -1);
};

QTDAO_END_NAMESPACE
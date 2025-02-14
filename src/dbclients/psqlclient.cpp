#include "dbclients/psqlclient.h"

#include "connectionpool.h"
#include "dbexception.h"
#include "query/basequery.h"
#include "config/configbuilder.h"

#include <qstringbuilder.h>
#include <qregularexpression.h>

QTDAO_BEGIN_NAMESPACE

#define PSQL_KEYWORDS_ESCAPES {"\""}

#define RL_TB(tb) checkAndRemoveKeywordEscapes(tb, PSQL_KEYWORDS_ESCAPES)

void PSqlClient::testConnect() {
    BaseQuery::executePrimitiveQuery("select 1", currentSessionId(), "psql");
}

void PSqlClient::createDatabase() {

}

void PSqlClient::dropDatabase() {}

QStringList PSqlClient::exportAllTables() {}

bool PSqlClient::checkTableExist(const QString &tbName) {}

void PSqlClient::createTableIfNotExist(const QString &tbName, const QStringList &fieldsType, const QStringList &primaryKeys, const QList<ForeignKey> &foreignKeys, const QString &engine) {}

void PSqlClient::renameTable(const QString &oldName, const QString &newName) {}

void PSqlClient::dropTable(const QString &tbName) {}

void PSqlClient::truncateTable(const QString &tbName) {}

void PSqlClient::enableForeignKey(const QString &tbName, bool enabled) {}

void PSqlClient::dropReferencedForeignKey(const QString &tbName) {}

QList<QPair<QString, QString>> PSqlClient::exportAllFields(const QString &tbName) {}

void PSqlClient::addField(const QString &tbName, const QString &field) {}

void PSqlClient::dropField(const QString &tbName, const QString &fieldName) {}

void PSqlClient::renameField(const QString &tbName, const QString &oldFieldName, const QString &newFieldName) {}

QHash<IndexType, QStringList> PSqlClient::exportAllIndexes(const QString &tbName) {}

void PSqlClient::createIndex(const QString &tbName, const QString &indexName, const QStringList &fields, IndexType type, const QString &indexOption) {}

void PSqlClient::dropIndex(const QString &tbName, const QString &indexName) {}

QString PSqlClient::getIndexFromFields(const QStringList &fields) {}

QString PSqlClient::createEscapeCharsForName(const QString &sourceName) const {}

QString PSqlClient::removeEscapeCharsForName(const QString &sourceName) const {}

QTDAO_END_NAMESPACE
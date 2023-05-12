#pragma once

#include "../global.h"
#include "../utils/dbupgrader.h"

#include <qobject.h>
#include <functional>
#include <type_traits>
#include <qscopedpointer.h>

QTDAO_BEGIN_NAMESPACE

class EntityReaderInterface;
class AbstractClient {
public:
    virtual void testConnect() = 0;

    virtual void createDatabase() = 0;

    virtual void dropDatabase() = 0;

    virtual bool checkTableExist(const QString& tbName) = 0;

    virtual void createTable(EntityReaderInterface* reader) = 0;

    virtual void renameTable(const QString& oldName, const QString& newName) = 0;

    virtual void dropTable(const QString& tbName) = 0;

    virtual void truncateTable(const QString& tbName) = 0;

    virtual QStringList getTagTableFields(const QString& tbName) = 0;

    virtual void restoreDataBefore(const QString& tbName);

    virtual void restoreDataAfter(const QString& tbName);

    virtual void dropAllIndexOnTable(const QString& tbName) = 0;

    virtual QString translateSqlStatement(const QString& statement, const QVariantList& values);

public:
    template<int Type, typename E>
    void createTable();

    template<int Type, typename E>
    void tableUpgrade(int oldVersion, int curVersion);

    void restoreData2NewTable(const QString& tbName, const QStringList& fields);

private:
    static void tableUpgrade(EntityReaderInterface* reader, int oldVersion, int curVersion);

    template<int Type, typename E>
    typename std::enable_if<Type == 0, EntityReaderInterface*>::type
    getEntityInfoProvider() {
        return new SqliteEntityReaderProvider<E>();
    }

    template<int Type, typename E>
    typename std::enable_if<Type == 1, EntityReaderInterface*>::type
    getEntityInfoProvider() {
        return new MysqlEntityReaderProvider<E>();
    }

    template<int Type, typename E>
    typename std::enable_if<Type == 2, EntityReaderInterface*>::type
    getEntityInfoProvider() {
        return new SqlServerEntityReaderProvider<E>();
    }

protected:
    static QString checkAndRemoveKeywordEscapes(const QString& tbOrFieldName, const QStringList& symbols);

    static QString currentDatabaseName();

    friend class ConfigBuilder;

    enum IndexType {
        //sqlite mysql
        INDEX_NORMAL,
        INDEX_UNIQUE,
        //sql server
        INDEX_CLUSTERED,
        INDEX_UNIQUE_CLUSTERED,
        INDEX_NONCLUSTERED,
        INDEX_UNIQUE_NONCLUSTERED,
    };
};

template<int Type, typename E>
inline void AbstractClient::createTable() {
    typename E::Info info;
    if (checkTableExist(info.getTableName())) {
        return;
    }
    auto reader = getEntityInfoProvider<Type, E>();
    createTable(reader);
    delete reader;
}

template<int Type, typename E>
inline void AbstractClient::tableUpgrade(int oldVersion, int curVersion) {
    auto reader = getEntityInfoProvider<Type, E>();
    tableUpgrade(reader, oldVersion, curVersion);
    delete reader;
}

QTDAO_END_NAMESPACE
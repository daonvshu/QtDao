#pragma once

#include "../global.h"
#include "../utils/dbupgrader.h"

#include <qobject.h>
#include <functional>
#include <type_traits>
#include <qscopedpointer.h>
#include <qhash.h>

QTDAO_BEGIN_NAMESPACE

enum class IndexType {
    //sqlite mysql
    INDEX_NORMAL,
    INDEX_UNIQUE,
    //sql server
    INDEX_CLUSTERED,
    INDEX_UNIQUE_CLUSTERED,
    INDEX_NONCLUSTERED,
    INDEX_UNIQUE_NONCLUSTERED,
};

#if QT_VERSION_MAJOR >= 6
inline size_t qHash(IndexType type, size_t seed) {
    return ::qHash((int)type, seed);
}
#else
inline uint qHash(IndexType type, uint seed) {
    return ::qHash((int)type, seed);
}
#endif

class EntityReaderInterface;
class AbstractClient {
public:
    /**
     * test connection use current config
     */
    virtual void testConnect() = 0;

    /////////////// database process ///////////////

    /**
     * create database
     */
    virtual void createDatabase() = 0;

    /**
     * drop database
     */
    virtual void dropDatabase() = 0;

    /////////////// table process ///////////////

    /**
     * export all tables of current configured database
     * @return table name
     */
    virtual QStringList exportAllTables() = 0;

    /**
     * check table exist in configured database
     * @param tbName the table name need to check exist
     * @return true is exist
     */
    virtual bool checkTableExist(const QString& tbName) = 0;

    /**
     * create table if not exist in configured database, use entity reader interface
     * @param reader SqliteEntityReaderProvider/MysqlEntityReaderProvider/SqlServerEntityReaderProvider
     */
    void createTableIfNotExist(EntityReaderInterface* reader);

    /**
     * create table if not exist in configured database
     * @param tbName the table name to create
     * @param fieldsType field definitions, for example: "name varchar default ''"
     * @param primaryKeys fields defined primary key
     * @param engine only used by mysql
     */
    virtual void createTableIfNotExist(
            const QString& tbName,
            const QStringList& fieldsType,
            const QStringList& primaryKeys,
            const QString& engine
    ) = 0;

    /**
     * rename table to new name
     * @param oldName old table name
     * @param newName new table name
     */
    virtual void renameTable(const QString& oldName, const QString& newName) = 0;

    /**
     * drop table
     * @param tbName table name
     */
    virtual void dropTable(const QString& tbName) = 0;

    /**
     * clear all data and reset auto-increment id
     * @param tbName table name
     */
    virtual void truncateTable(const QString& tbName) = 0;

    /////////////// table field process ///////////////

    /**
     * export the target table current all fields
     * @param tbName table name
     * @return all field, pair<field name, field type>
     */
    virtual QList<QPair<QString, QString>> exportAllFields(const QString& tbName) = 0;

    /**
     * add new column in target table
     * @param tbName table name
     * @param field field definition, for example: "name varchar default ''"
     */
    virtual void addField(const QString& tbName, const QString& field) = 0;

    /**
     * remove column in target table.
     * in sqlite, unsupported to drop column with primary key or unique constraint,
     * and at least Qt5.15.4 version.
     * @param tbName table name
     * @param fieldName field name
     */
    virtual void dropField(const QString& tbName, const QString& fieldName) = 0;

    /**
     * rename column name
     * @param tbName table name
     * @param oldFieldName old name
     * @param newFieldName new name
     */
    virtual void renameField(const QString& tbName, const QString& oldFieldName, const QString& newFieldName) = 0;

    /////////////// table index process ///////////////

    /**
     * export the target table current all index
     * @param tbName table name
     * @return index type -> index array
     */
    virtual QHash<IndexType, QStringList> exportAllIndexes(const QString& tbName) = 0;

    /**
     * remove all index in target table
     * @param tbName table name
     */
    void dropAllIndexOnTable(const QString& tbName);

    /**
     * create index for table, use entity reader interface
     * @param reader SqliteEntityReaderProvider/MysqlEntityReaderProvider/SqlServerEntityReaderProvider
     */
    void createIndex(EntityReaderInterface* reader);

    /**
     * create index for table, used fields to create
     * @param tbName table name
     * @param fields used fields of index
     * @param type index type
     * @param indexOption index option, only used by SQLServer
     */
    void createIndex(const QString& tbName,
                     const QStringList& fields,
                     IndexType type,
                     const std::function<QString(const QString&)>& indexOption = nullptr);

    /**
     * create index for table
     * @param tbName table name
     * @param indexName index name
     * @param fields used fields of index
     * @param type index type
     * @param indexOption index option, only used by SQLServer
     */
    virtual void createIndex(const QString& tbName,
                             const QString& indexName,
                             const QStringList& fields,
                             IndexType type,
                             const QString& indexOption) = 0;

    /**
     * remove index by used fields in table
     * @param tbName table name
     * @param fields used fields
     */
    void dropIndex(const QString& tbName, const QStringList& fields);

    /**
     * remove index by name in table
     * @param tbName table name
     * @param indexName index name
     */
    virtual void dropIndex(const QString& tbName, const QString& indexName) = 0;

    /**
     * create index name by used fields
     * @param fields used fields
     * @return index name
     */
    virtual QString getIndexFromFields(const QStringList& fields) = 0;

    /**
     * create index name by used fields
     * @param fieldArray array of used fields
     * @return index name array
     */
    QStringList getIndexArrayFromFields(const QList<QStringList>& fieldArray);

    /////////////// table data process ///////////////

    /**
     * table data transfer from 'fromTb' to 'toTb'
     * @param fromTb from table name
     * @param toTb to table name
     */
    virtual void transferData(const QString& fromTb, const QString& toTb);

    /**
     * replace value and format in sql statement
     * @param statement sql statement
     * @param values execute values
     * @return formatted sql statement
     */
    virtual QString translateSqlStatement(const QString& statement, const QVariantList& values);

public:
    template<int Type, typename E>
    void createTable();

    template<int Type, typename E>
    void tableUpgrade(int oldVersion, int curVersion);

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

public:
    /**
     * use name in strings must remove keyword escapes
     * @param tbOrFieldName table name or field name
     * @param symbols keyword escape symbols
     * @return real name
     */
    static QString checkAndRemoveKeywordEscapes(const QString& tbOrFieldName, const QStringList& symbols);

    /**
     * get index name from used fields, removed keyword escapes use 'escapeSymbols'
     * @param fields used fields of index
     * @param escapeSymbols keyword escape symbols
     * @return index name
     */
    static QString getIndexFromFields(const QStringList& fields, const QStringList& escapeSymbols);

protected:
    static QString currentDatabaseName();

    friend class ConfigBuilder;
};

template<int Type, typename E>
inline void AbstractClient::createTable() {
    typename E::Info info;
    if (checkTableExist(info.getTableName())) {
        return;
    }
    auto reader = getEntityInfoProvider<Type, E>();
    createTableIfNotExist(reader);
    createIndex(reader);
    delete reader;
}

template<int Type, typename E>
inline void AbstractClient::tableUpgrade(int oldVersion, int curVersion) {
    auto reader = getEntityInfoProvider<Type, E>();
    tableUpgrade(reader, oldVersion, curVersion);
    delete reader;
}

QTDAO_END_NAMESPACE
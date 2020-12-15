#pragma once

#include <qobject.h>

class AbstractClient {
public:
    virtual void testConnect() = 0;
    virtual void createDatabase() = 0;
    virtual void dropDatabase() = 0;

    virtual bool checkTableExist(const QString& tbName) = 0;
    virtual void createTableIfNotExist(
        const QString& tbName,
        QStringList fieldsType,
        QStringList primaryKeys
    ) = 0;

    enum IndexType {
        INDEX_NORMAL,
        INDEX_UNIQUE,
    };

    virtual void createIndex(
        const QString& tbName,
        QStringList fields,
        IndexType type = INDEX_NORMAL
    ) = 0;

    virtual void renameTable(const QString& oldName, const QString& newName) = 0;

    virtual void dropTable(const QString& tbName) = 0;

    virtual void truncateTable(const QString& tbName) = 0;

    virtual QStringList getTagTableFields(const QString& tbName) = 0;

    virtual void restoreDataBefore(const QString& tbName)  { Q_UNUSED(tbName) }

    virtual void restoreDataAfter(const QString& tbName) { Q_UNUSED(tbName) }

    virtual void dropAllIndexOnTable(const QString& tbName) = 0;

private:
    template<typename... E> class Delegate;
    template<typename T, typename... E>
    class Delegate<T, E...> : public Delegate<E...> {
    public:
        using Delegate<E...>::Delegate;

        void createTable();
        void tableUpgrade();
    };

public:
    template<typename... E>
    void createTables();

    template<typename... E>
    void tablesUpgrade();

private:
    template<typename E>
    void createTable();

    template<typename E>
    void tableUpgrade();

    void restoreData2NewTable(const QString& tbname, QStringList fields);
};

template<> class AbstractClient::Delegate<> {
public:
    Delegate(AbstractClient* client) : client(client) {}

    void createTable() {}
    void tableUpgrade() {};

protected:
    AbstractClient* client;
};

/////////////////////////////// delegate //////////////////////////////////
template<typename ...E>
inline void AbstractClient::createTables() {
    Delegate<E...>(this).createTable();
}

template<typename ...E>
inline void AbstractClient::tablesUpgrade() {
    Delegate<E...>(this).tableUpgrade();
}

////////////////////////////// implement //////////////////////////////////
template<typename E>
inline void AbstractClient::createTable() {
    typename E::Info info;
    if (!checkTableExist(info.getTableName())) {
        //create table
        createTableIfNotExist(info.getTableName(), info.getFieldsType(), info.getPrimaryKeys());
        //create normal index
        auto indexFields = info.getIndexFields();
        for (const auto& i : indexFields) {
            createIndex(info.getTableName(), i);
        }
        //create unique index
        auto uniqueIndexFields = info.getUniqueIndexFields();
        for (const auto& i : uniqueIndexFields) {
            createIndex(info.getTableName(), i, INDEX_UNIQUE);
        }
    }
}

template<typename E>
inline void AbstractClient::tableUpgrade() {
    typename E::Info info;
    dropTable("tmp_" + info.getTableName());
    dropAllIndexOnTable(info.getTableName()); //clear indexes!
    renameTable(info.getTableName(), "tmp_" + info.getTableName());
    createTable<E>();
    restoreData2NewTable(info.getTableName(), info.getFields());
    dropTable("tmp_" + info.getTableName());
}

/////////////////////////////// delegate //////////////////////////////////
template<typename T, typename ...E>
inline void AbstractClient::Delegate<T, E...>::createTable() {
    AbstractClient::Delegate<>::client->createTable<T>();
    Delegate<E...>::createTable();
}

template<typename T, typename ...E>
inline void AbstractClient::Delegate<T, E...>::tableUpgrade() {
    AbstractClient::Delegate<>::client->tableUpgrade<T>();
    Delegate<E...>::tableUpgrade();
}

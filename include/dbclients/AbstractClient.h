#pragma once

#include <qobject.h>

enum SqlClientList {
    ClientSqlite,
    ClientMysql,
    ClientSqlServer,
};

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
    );
    virtual void createTableIfNotExist(
        const QString& tbName,
        const QString& engine,
        QStringList fieldsType,
        QStringList primaryKeys
    );

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

    virtual QString translateSqlStatement(const QString& statement, const QVariantList& values);

private:
    template<SqlClientList C, typename... E> class Delegate;
    template<SqlClientList C, typename T, typename... E>
    class Delegate<C, T, E...> : public Delegate<C, E...> {
    public:
        using Delegate<C, E...>::Delegate;

        void createTable();
        void tableUpgrade();
    };

    template<SqlClientList C>
    class ClientSelector {
    public:
        ClientSelector(AbstractClient* client) : client(client) {}

        template<typename E>
        void createTableIfNotExist();
        
    protected:
        AbstractClient* client;
    };

public:
    template<SqlClientList C, typename... E>
    void createTables();

    template<SqlClientList C, typename... E>
    void tablesUpgrade();

private:
    template<SqlClientList C, typename E>
    void createTable();

    template<SqlClientList C, typename E>
    void tableUpgrade();

    void restoreData2NewTable(const QString& tbname, QStringList fields);
};

template<SqlClientList C> class AbstractClient::Delegate<C> {
public:
    Delegate(AbstractClient* client) : client(client) {}

    void createTable() {}
    void tableUpgrade() {};

protected:
    AbstractClient* client;
};

/////////////////////////////// delegate //////////////////////////////////
template<SqlClientList C, typename ...E>
inline void AbstractClient::createTables() {
    Delegate<C, E...>(this).createTable();
}

template<SqlClientList C, typename ...E>
inline void AbstractClient::tablesUpgrade() {
    Delegate<C, E...>(this).tableUpgrade();
}

////////////////////////////// implement //////////////////////////////////
template<SqlClientList C, typename E>
inline void AbstractClient::createTable() {
    typename E::Info info;
    if (!checkTableExist(info.getTableName())) {
        //create table
        ClientSelector<C>(this).template createTableIfNotExist<E>();
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

template<SqlClientList C, typename E>
inline void AbstractClient::tableUpgrade() {
    typename E::Info info;
    dropTable("tmp_" + info.getTableName());
    dropAllIndexOnTable(info.getTableName()); //clear indexes!
    renameTable(info.getTableName(), "tmp_" + info.getTableName());
    createTable<C, E>();
    restoreData2NewTable(info.getTableName(), info.getFields());
    dropTable("tmp_" + info.getTableName());
}

/////////////////////////////// delegate //////////////////////////////////
template<SqlClientList C, typename T, typename ...E>
inline void AbstractClient::Delegate<C, T, E...>::createTable() {
    AbstractClient::Delegate<C>::client->template createTable<C, T>();
    Delegate<C, E...>::createTable();
}

template<SqlClientList C, typename T, typename ...E>
inline void AbstractClient::Delegate<C, T, E...>::tableUpgrade() {
    AbstractClient::Delegate<C>::client->template tableUpgrade<C, T>();
    Delegate<C, E...>::tableUpgrade();
}

template<>
template<typename E>
inline void AbstractClient::ClientSelector<ClientSqlite>::createTableIfNotExist() {
    typename E::Info info;
    client->createTableIfNotExist(info.getTableName(), info.getFieldsType(), info.getPrimaryKeys());
}

template<>
template<typename E>
inline void AbstractClient::ClientSelector<ClientMysql>::createTableIfNotExist() {
    typename E::Info info;
    client->createTableIfNotExist(info.getTableName(), info.getTableEngine(), info.getFieldsType(), info.getPrimaryKeys());
}

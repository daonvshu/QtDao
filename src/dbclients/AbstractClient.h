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

private:
    template<typename... E> struct Delegate;
    template<typename T, typename... E>
    class Delegate<T, E...> : public Delegate<E...> {
    public:
        using Delegate<E...>::Delegate;

        void createTable();
    };

public:
    template<typename... E>
    void createTables();

private:
    template<typename E>
    void createTable();
};

template<> class AbstractClient::Delegate<> {
public:
    Delegate(AbstractClient* client) : client(client) {}

    void createTable() {}

protected:
    AbstractClient* client;
};

template<typename ...E>
inline void AbstractClient::createTables() {
    Delegate<E...>(this).createTable();
}

template<typename E>
inline void AbstractClient::createTable() {
    typename E::Info info;
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

template<typename T, typename ...E>
inline void AbstractClient::Delegate<T, E...>::createTable() {
    client->createTable<T>();
    Delegate<E...>::createTable();
}

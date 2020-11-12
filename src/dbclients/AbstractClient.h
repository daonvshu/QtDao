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
        QStringList fieldsType
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
    createTableIfNotExist(info.getTableName(), info.getFieldsType());
}

template<typename T, typename ...E>
inline void AbstractClient::Delegate<T, E...>::createTable() {
    client->createTable<T>();
    Delegate<E...>::createTable();
}

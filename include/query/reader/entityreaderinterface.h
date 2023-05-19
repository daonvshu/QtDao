#pragma once

#include "global.h"
#include "utils/listutils.h"

#include <qvariant.h>

#pragma warning(disable:4250)
QTDAO_BEGIN_NAMESPACE

class EntityReaderInterface {
public:
    virtual ~EntityReaderInterface() = default;

    virtual int fieldSize() = 0;
    
    virtual QString getTableName() = 0;

    virtual QString getSourceName() = 0;

    virtual QStringList getFields() = 0;

    virtual QStringList getFieldsWithoutAutoIncrement() = 0;

    virtual QStringList getFieldsType() = 0;

    virtual QStringList getPrimaryKeys() = 0;

    virtual bool isAutoIncrement(const QString& fieldName) = 0;

    //extra info reader

    virtual QList<QStringList> getIndexFields() = 0; //sqlite/mysql

    virtual QList<QStringList> getUniqueIndexFields() = 0; //sqlite/mysql

    virtual QString getTableEngine() = 0; //mysql

    virtual QList<QStringList> getClusteredIndexFields() = 0; //sqlserver

    virtual QList<QStringList> getUniqueClusteredIndexFields() = 0; //sqlserver

    virtual QList<QStringList> getNonClusteredIndexFields() = 0; //sqlserver

    virtual QList<QStringList> getUniqueNonClusteredIndexFields() = 0; //sqlserver

    virtual QString getIndexOption(const QString& name) = 0; //sqlserver

    //extra functions

    template<typename E>
    bool isTable() {
        return getTableName() == E::Info::getTableName();
    }
};

template<typename E>
class EntityReaderProvider : public virtual EntityReaderInterface {
public:
    int fieldSize() override {
        return E::Info::fieldSize();
    }
    
    QString getTableName() override {
        return E::Info::getTableName();
    }

    QString getSourceName() override {
        return E::Info::getSourceName();
    }

    QStringList getFields() override {
        return E::Info::getFields();
    }

    QStringList getFieldsWithoutAutoIncrement() override {
        return E::Info::getFieldsWithoutAutoIncrement();
    }

    QStringList getFieldsType() override {
        return E::Info::getFieldsType();
    }

    QStringList getPrimaryKeys() override {
        return E::Info::getPrimaryKeys();
    }

    bool isAutoIncrement(const QString& name) override {
        return E::Info::isAutoIncrement(name);
    }

    QVariantList getValueWithoutAutoIncrement(const E& entity) {
        return E::Tool::getValueWithoutAutoIncrement(entity);
    }

    QVariant getValueByName(const E& entity, const QString& target) {
        return E::Tool::getValueByName(entity, target);
    }

    void bindAutoIncrementId(E& entity, const QVariant& id) {
        E::Tool::bindAutoIncrementId(entity, id);
    }

    void bindValue(E& entity, const QString& target, QVariant value) {
        E::Tool::bindValue(entity, target, value);
    }

    //extra

    QList<QStringList> getIndexFields() override {
        return {};
    }

    QList<QStringList> getUniqueIndexFields() override {
        return {};
    }

    QString getTableEngine() override {
        return {};
    }

    QList<QStringList> getClusteredIndexFields() override {
        return {};
    }

    QList<QStringList> getUniqueClusteredIndexFields() override {
        return {};
    }

    QList<QStringList> getNonClusteredIndexFields() override {
        return {};
    }

    QList<QStringList> getUniqueNonClusteredIndexFields() override {
        return {};
    }

    QString getIndexOption(const QString& name) override {
        return {};
    }
};

template<typename E>
class SqliteEntityReaderProvider : public EntityReaderProvider<E> {
public:
    QList<QStringList> getIndexFields() override {
        return E::Info::getIndexFields();
    }

    QList<QStringList> getUniqueIndexFields() override {
        return E::Info::getUniqueIndexFields();
    }
};

template<typename E>
class MysqlEntityReaderProvider : public EntityReaderProvider<E> {
public:
    QList<QStringList> getIndexFields() override {
        return E::Info::getIndexFields();
    }

    QList<QStringList> getUniqueIndexFields() override {
        return E::Info::getUniqueIndexFields();
    }

    QString getTableEngine() override {
        return E::Info::getTableEngine();
    }
};

template<typename E>
class SqlServerEntityReaderProvider : public EntityReaderProvider<E> {
public:
    QList<QStringList> getClusteredIndexFields() override {
        return E::Info::getClusteredIndexFields();
    }

    QList<QStringList> getUniqueClusteredIndexFields() override {
        return E::Info::getUniqueClusteredIndexFields();
    }

    QList<QStringList> getNonClusteredIndexFields() override {
        return E::Info::getNonClusteredIndexFields();
    }

    QList<QStringList> getUniqueNonClusteredIndexFields() override {
        return E::Info::getUniqueNonClusteredIndexFields();
    }

    QString getIndexOption(const QString& name) override {
        return E::Info::getIndexOption(name);
    }
};

QTDAO_END_NAMESPACE
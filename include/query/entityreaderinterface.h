#pragma once

#include "../global.h"
#include "../utils/listutils.h"

#pragma warning(disable:4250)
QTDAO_BEGIN_NAMESPACE

class EntityReaderInterface {
protected:
    virtual int fieldSize() = 0;
    
    virtual QString getTableName() = 0;

    virtual QString getSourceName() = 0;

    virtual QStringList getFields() = 0;

    virtual QStringList getFieldsWithoutAutoIncrement() = 0;

    virtual QStringList getFieldsType() = 0;

    virtual QStringList getPrimaryKeys() = 0;

    virtual bool isAutoIncrement(const QString& fieldName) = 0;

    virtual QString getTableEngine() = 0;

    virtual QStringList getFieldsWithoutTimestamp() = 0;
};

template<typename E>
class EntityReaderProvider : protected virtual EntityReaderInterface {
protected:
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

    //mysql
    QString getTableEngine() override {
        return E::Info::getTableEngine();
    }

    //sqlserver
    QStringList getFieldsWithoutTimestamp() override {
        return E::Info::getFieldsWithoutTimestamp();
    }
};

QTDAO_END_NAMESPACE
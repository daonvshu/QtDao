﻿/*This file is auto generated by DbEntityGenerator*/
#pragma once

#include <qobject.h>
#include <qvariant.h>
#include <qjsonobject.h>

#include "condition/EntityField.h"

class SqliteTest1 {
private:
    //主键
    qint64 id;
    //复合主键
    QString name;
    //浮点数字
    qreal number;
    //二进制
    QByteArray hex;

    QHash<QString, QVariant> __extra;

public:
    SqliteTest1() {
        id = -1;
        name = "text";
        number = 10.0;
    }

    SqliteTest1(
        const qint64& id,
        const QString& name,
        const qreal& number,
        const QByteArray& hex
    ) : id(id)
    , name(name)
    , number(number)
    , hex(hex)
    {
    }

    SqliteTest1(
        const QByteArray& hex
    ) : hex(hex)
    {
        id = -1;
        name = "text";
        number = 10.0;
    }

public:
    class Fields {
    public:
        EntityField<qint64> id = EntityField<qint64>("id", "ts_sqlitetest1");
        EntityField<QString> name = EntityField<QString>("name", "ts_sqlitetest1");
        EntityField<qreal> number = EntityField<qreal>("number", "ts_sqlitetest1");
        EntityField<QByteArray> hex = EntityField<QByteArray>("hex", "ts_sqlitetest1");

    protected:
        void reset(const QString& tbName) {
            id = EntityField<qint64>("id", tbName);
            name = EntityField<QString>("name", tbName);
            number = EntityField<qreal>("number", tbName);
            hex = EntityField<QByteArray>("hex", tbName);
        }
    };

    struct Info {
        enum {
            Attach = 1
        };

        static int fieldSize() {
            return 4;
        }

        static QString getTableName() {
            return QStringLiteral("ts_sqlitetest1");
        }

        static QString getSourceName() {
            return getTableName();
        }

        static QStringList getFields() {
            return QStringList()
                << "id"
                << "name"
                << "number"
                << "hex";
        }

        static QStringList getFieldsWithoutAutoIncrement() {
            return QStringList()
                << "id"
                << "name"
                << "number"
                << "hex";
        }

        static QStringList getFieldsType() {
            return QStringList() 
                << QStringLiteral("id integer default -1")
                << QStringLiteral("name text default 'text'")
                << QStringLiteral("number real default 10.0")
                << QStringLiteral("hex blob");
        }

        static QStringList getPrimaryKeys() {
            return QStringList() << "id" << "name";
        }

        static QList<QStringList> getIndexFields() {
            return QList<QStringList>();
        }

        static QList<QStringList> getUniqueIndexFields() {
            return QList<QStringList>();
        }

        static bool isAutoIncrement(const QString& name) {
            Q_UNUSED(name);
            return false;
        }
    };

    struct Tool {
        static QVariantList getValueWithoutAutoIncrement(const SqliteTest1& entity) {
            return QVariantList()
                << entity.id
                << entity.name
                << entity.number
                << entity.hex;
        }

        static QVariant getValueByName(const SqliteTest1& entity, const QString& target) {
            if (target == "id") {
                return entity.id;
            }
            if (target == "name") {
                return entity.name;
            }
            if (target == "number") {
                return entity.number;
            }
            if (target == "hex") {
                return entity.hex;
            }
            return entity.__extra.value(target);
        }

        static void bindAutoIncrementId(SqliteTest1& entity, const QVariant& id) {
            Q_UNUSED(entity);
            Q_UNUSED(id);
        }

        static void bindValue(SqliteTest1& entity, const QString& target, QVariant value) {
            if (target == "id") {
                entity.id = value.value<qint64>();
            } else if (target == "name") {
                entity.name = value.value<QString>();
            } else if (target == "number") {
                entity.number = value.value<qreal>();
            } else if (target == "hex") {
                entity.hex = value.value<QByteArray>();
            } else {
                entity.__putExtra(target, value);
            }
        }

        static SqliteTest1 fromJson(const QJsonObject& object) {
            SqliteTest1 entity;
            entity.id = object.value("id").toVariant().value<qint64>();
            entity.name = object.value("name").toVariant().value<QString>();
            entity.number = object.value("number").toVariant().value<qreal>();
            entity.hex = QByteArray::fromBase64(object.value("hex").toString().toLatin1());
            return entity;
        }

        static QJsonObject toJson(const SqliteTest1& entity, QStringList excludeKeys = QStringList()) {
            QJsonObject object;
            object.insert("id", entity.id);
            object.insert("name", entity.name);
            object.insert("number", entity.number);
            object.insert("hex", QString::fromLatin1(entity.hex.toBase64()));

            for (const auto& key : excludeKeys) {
                object.remove(key);
            }
            return object;
        }
    };

public:
    //set 主键
    inline void setId(const qint64& id) {this->id = id;}
    //get 主键
    inline qint64 getId() const {return id;}
    //set 复合主键
    inline void setName(const QString& name) {this->name = name;}
    //get 复合主键
    inline QString getName() const {return name;}
    //set 浮点数字
    inline void setNumber(const qreal& number) {this->number = number;}
    //get 浮点数字
    inline qreal getNumber() const {return number;}
    //set 二进制
    inline void setHex(const QByteArray& hex) {this->hex = hex;}
    //get 二进制
    inline QByteArray getHex() const {return hex;}
    //set temp data
    inline void __putExtra(const QString& key, const QVariant& extra) {this->__extra.insert(key, extra);}
    //get function select result, like get "as" field result
    inline QVariant __getExtra(const QString& key) const {return __extra.value(key);}
};
typedef QList<SqliteTest1> SqliteTest1List;
Q_DECLARE_METATYPE(SqliteTest1);
﻿/*This file is auto generated by vscode-qtdao*/
#pragma once

#include <qobject.h>
#include <qvariant.h>
#include <qdatetime.h>
#include <qjsonobject.h>

#include "condition/entityfield.h"

class SqliteTest2 {
    Q_GADGET

    Q_PROPERTY(qint64 id MEMBER id)
    Q_PROPERTY(QString name MEMBER name)
    Q_PROPERTY(int number MEMBER number)
    Q_PROPERTY(int number2 MEMBER number2)
    Q_PROPERTY(QVariant varianttype MEMBER varianttype)
    Q_PROPERTY(QString nametmp MEMBER nametmp)
    Q_PROPERTY(QVariantMap extra MEMBER __extra)

private:
    //自增长主键
    qint64 id;
    //
    QString name;
    //
    int number;
    //
    int number2;
    //自定义类型
    QVariant varianttype;

    ///transient 临时类型
    QString nametmp;

    QVariantMap __extra;

public:

   SqliteTest2() {
        id = -1;
        number = 0;
   }

    SqliteTest2(
        const QString& name,
        const int& number,
        const int& number2,
        const QVariant& varianttype
    ) : name(name)
    , number(number)
    , number2(number2)
    , varianttype(varianttype)
    {
        id = -1;
    }

    SqliteTest2(
        const QString& name,
        const int& number2,
        const QVariant& varianttype
    ) : name(name)
    , number2(number2)
    , varianttype(varianttype)
    {
        id = -1;
        number = 0;
    }

public:
    class Fields {
    public:
        dao::EntityField<qint64> id = dao::EntityField<qint64>("id", "ts_sqlitetest2");
        dao::EntityField<QString> name = dao::EntityField<QString>("name", "ts_sqlitetest2");
        dao::EntityField<int> number = dao::EntityField<int>("number", "ts_sqlitetest2");
        dao::EntityField<int> number2 = dao::EntityField<int>("number2", "ts_sqlitetest2");
        dao::EntityField<QVariant> varianttype = dao::EntityField<QVariant>("varianttype", "ts_sqlitetest2");

    protected:
        void reset(const QString& tbName) {
            id = dao::EntityField<qint64>("id", tbName);
            name = dao::EntityField<QString>("name", tbName);
            number = dao::EntityField<int>("number", tbName);
            number2 = dao::EntityField<int>("number2", tbName);
            varianttype = dao::EntityField<QVariant>("varianttype", tbName);
        }
    };

    struct Info {
        enum {
            Attach = 1
        };

        static int fieldSize() {
            return 5;
        }

        static QString getTableName() {
            return QStringLiteral("ts_sqlitetest2");
        }

        static QString getSourceName() {
            return getTableName();
        }

        static QStringList getFields() {
            return QStringList()
                << "id"
                << "name"
                << "number"
                << "number2"
                << "varianttype";
        }

        static QStringList getFieldsWithoutAutoIncrement() {
            return QStringList()
                << "name"
                << "number"
                << "number2"
                << "varianttype";
        }

        static QStringList getFieldsType() {
            return QStringList() 
                << QStringLiteral("id integer primary key autoincrement")
                << QStringLiteral("name text not null")
                << QStringLiteral("number integer null default 0")
                << QStringLiteral("number2 integer")
                << QStringLiteral("varianttype blob");
        }

        static QStringList getPrimaryKeys() {
            return QStringList() << "id";
        }

        static QList<QStringList> getIndexFields() {
            return QList<QStringList>()
                << (QStringList() << "number2")
                << (QStringList() << "number" << "number2");
        }

        static QList<QStringList> getUniqueIndexFields() {
            return QList<QStringList>()
                << (QStringList() << "name asc" << "number desc");
        }

        static bool isAutoIncrement(const QString& name) {
            return name == "id";
        }
    };

    struct Tool {
        static QVariantList getValueWithoutAutoIncrement(const SqliteTest2& entity) {
            return QVariantList()
                << entity.name
                << entity.number
                << entity.number2
                << entity.varianttype;
        }

        static QVariant getValueByName(const SqliteTest2& entity, const QString& target) {
            if (target == "id") {
                return entity.id;
            }
            if (target == "name") {
                return entity.name;
            }
            if (target == "number") {
                return entity.number;
            }
            if (target == "number2") {
                return entity.number2;
            }
            if (target == "varianttype") {
                return entity.varianttype;
            }
            return entity.__extra.value(target);
        }

        static void bindAutoIncrementId(SqliteTest2& entity, const QVariant& id) {
            entity.id = id.value<qint64>();
        }

        static void bindValue(SqliteTest2& entity, const QString& target, QVariant value) {
            if (target == "id") {
                entity.id = value.value<qint64>();
            } else if (target == "name") {
                entity.name = value.value<QString>();
            } else if (target == "number") {
                entity.number = value.value<int>();
            } else if (target == "number2") {
                entity.number2 = value.value<int>();
            } else if (target == "varianttype") {
                entity.varianttype = value.value<QVariant>();
            } else {
                entity.__putExtra(target, value);
            }
        }

        static SqliteTest2 fromJson(const QJsonObject& object) {
            SqliteTest2 entity;
            entity.id = object.value("id").toVariant().value<qint64>();
            entity.name = object.value("name").toVariant().value<QString>();
            entity.number = object.value("number").toVariant().value<int>();
            entity.number2 = object.value("number2").toVariant().value<int>();
            entity.varianttype = object.value("varianttype");
            return entity;
        }

        static QJsonObject toJson(const SqliteTest2& entity, QStringList excludeKeys = QStringList()) {
            QJsonObject object;
            object.insert("id", entity.id);
            object.insert("name", entity.name);
            object.insert("number", entity.number);
            object.insert("number2", entity.number2);
            object.insert("varianttype", QJsonValue::fromVariant(entity.varianttype));

            for (const auto& key : excludeKeys) {
                object.remove(key);
            }
            return object;
        }
    };

public:
    //set 自增长主键
    inline void setId(const qint64& id) {this->id = id;}
    //get 自增长主键
    inline qint64 getId() const {return id;}
    //
    inline void setName(const QString& name) {this->name = name;}
    //
    inline QString getName() const {return name;}
    //
    inline void setNumber(const int& number) {this->number = number;}
    //
    inline int getNumber() const {return number;}
    //
    inline void setNumber2(const int& number2) {this->number2 = number2;}
    //
    inline int getNumber2() const {return number2;}
    //set 自定义类型
    inline void setVarianttype(const QVariant& varianttype) {this->varianttype = varianttype;}
    //get 自定义类型
    inline QVariant getVarianttype() const {return varianttype;}
    //set 临时类型
    inline void setNametmp(const QString& nametmp) {this->nametmp = nametmp;}
    //get 临时类型
    inline QString getNametmp() const {return nametmp;}
    //set temp data
    inline void __putExtra(const QString& key, const QVariant& extra) {this->__extra.insert(key, extra);}
    //get function select result, like get "as" field result
    inline QVariant __getExtra(const QString& key) const {return __extra.value(key);}
};
typedef QList<SqliteTest2> SqliteTest2List;
Q_DECLARE_METATYPE(SqliteTest2);
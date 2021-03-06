﻿/*This file is auto generated by DbEntityGenerator*/
#pragma once

#include <qobject.h>
#include <qvariant.h>
#include <qdatetime.h>
#include <qjsonobject.h>

#include "condition/EntityField.h"

class MysqlTest2 {
private:
    //自增长主键
    qint64 id;
    //
    QString name;
    //
    int number;
    //
    int number2;

    ///transient 临时类型
    QString nametmp;

    QHash<QString, QVariant> __extra;

public:

    MysqlTest2() {
        id = -1;
        number = 0;
    }

    MysqlTest2(
        const QString& name,
        const int& number,
        const int& number2
    ) : name(name)
    , number(number)
    , number2(number2)
    {
        id = -1;
    }

    MysqlTest2(
        const QString& name,
        const int& number2
    ) : name(name)
    , number2(number2)
    {
        id = -1;
        number = 0;
    }

public:
    class Fields {
    public:
        EntityField<qint64> id = EntityField<qint64>("id", "ts_mysqltest2");
        EntityField<QString> name = EntityField<QString>("name", "ts_mysqltest2");
        EntityField<int> number = EntityField<int>("number", "ts_mysqltest2");
        EntityField<int> number2 = EntityField<int>("number2", "ts_mysqltest2");

    protected:
        void reset(const QString& tbName) {
            id = EntityField<qint64>("id", tbName);
            name = EntityField<QString>("name", tbName);
            number = EntityField<int>("number", tbName);
            number2 = EntityField<int>("number2", tbName);
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
            return QStringLiteral("ts_mysqltest2");
        }

        static QString getSourceName() {
            return getTableName();
        }

        static QString getTableEngine() {
            return "InnoDB";
        }

        static QStringList getFields() {
            return QStringList()
                << "id"
                << "name"
                << "number"
                << "number2";
        }

        static QStringList getFieldsWithoutAutoIncrement() {
            return QStringList()
                << "name"
                << "number"
                << "number2";
        }

        static QStringList getFieldsType() {
            return QStringList() 
                << QStringLiteral("id bigint primary key auto_increment comment '自增长主键'")
                << QStringLiteral("name varchar(100) not null comment ''")
                << QStringLiteral("number int null default 0 comment ''")
                << QStringLiteral("number2 int comment ''");
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
        static QVariantList getValueWithoutAutoIncrement(const MysqlTest2& entity) {
            return QVariantList()
                << entity.name
                << entity.number
                << entity.number2;
        }

        static QVariant getValueByName(const MysqlTest2& entity, const QString& target) {
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
            return entity.__extra.value(target);
        }

        static void bindAutoIncrementId(MysqlTest2& entity, const QVariant& id) {
            entity.id = id.value<qint64>();
        }

        static void bindValue(MysqlTest2& entity, const QString& target, QVariant value) {
            if (target == "id") {
                entity.id = value.value<qint64>();
            } else if (target == "name") {
                entity.name = value.value<QString>();
            } else if (target == "number") {
                entity.number = value.value<int>();
            } else if (target == "number2") {
                entity.number2 = value.value<int>();
            } else {
                entity.__putExtra(target, value);
            }
        }

        static MysqlTest2 fromJson(const QJsonObject& object) {
            MysqlTest2 entity;
            entity.id = object.value("id").toVariant().value<qint64>();
            entity.name = object.value("name").toVariant().value<QString>();
            entity.number = object.value("number").toVariant().value<int>();
            entity.number2 = object.value("number2").toVariant().value<int>();
            return entity;
        }

        static QJsonObject toJson(const MysqlTest2& entity, QStringList excludeKeys = QStringList()) {
            QJsonObject object;
            object.insert("id", entity.id);
            object.insert("name", entity.name);
            object.insert("number", entity.number);
            object.insert("number2", entity.number2);

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
    //set 临时类型
    inline void setNametmp(const QString& nametmp) {this->nametmp = nametmp;}
    //get 临时类型
    inline QString getNametmp() const {return nametmp;}
    //set temp data
    inline void __putExtra(const QString& key, const QVariant& extra) {this->__extra.insert(key, extra);}
    //get function select result, like get "as" field result
    inline QVariant __getExtra(const QString& key) const {return __extra.value(key);}
};
typedef QList<MysqlTest2> MysqlTest2List;
Q_DECLARE_METATYPE(MysqlTest2);
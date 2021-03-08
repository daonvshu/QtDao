﻿/*This file is auto generated by DbEntityGenerator*/
#pragma once

#include <qobject.h>
#include <qvariant.h>
#include <qdatetime.h>
#include <qjsonobject.h>

#include "condition/EntityField.h"

class SqlServerTest3 {
private:
    //
    qint64 id;
    //绑定到SqlServerTest1 id
    qint64 tbi1;
    //绑定到SqlServerTest2 id
    qint64 tbi2;
    //
    QString name;
    //
    int size;

    QHash<QString, QVariant> __extra;

public:

    SqlServerTest3() {
        id = -1;
        tbi1 = -1;
        tbi2 = -1;
    }

    SqlServerTest3(
        const qint64& tbi1,
        const qint64& tbi2,
        const QString& name,
        const int& size
    ) : tbi1(tbi1)
    , tbi2(tbi2)
    , name(name)
    , size(size)
    {
        id = -1;
    }

    SqlServerTest3(
        const QString& name,
        const int& size
    ) : name(name)
    , size(size)
    {
        id = -1;
        tbi1 = -1;
        tbi2 = -1;
    }

    SqlServerTest3(
        const qint64& tbi1,
        const qint64& tbi2,
        const QString& name
    ) : tbi1(tbi1)
    , tbi2(tbi2)
    , name(name)
    {
        id = -1;
    }

public:
    class Fields {
    public:
        EntityField<qint64> id = EntityField<qint64>("id", "ts_sqlservertest3");
        EntityField<qint64> tbi1 = EntityField<qint64>("tbi1", "ts_sqlservertest3");
        EntityField<qint64> tbi2 = EntityField<qint64>("tbi2", "ts_sqlservertest3");
        EntityField<QString> name = EntityField<QString>("name", "ts_sqlservertest3");
        EntityField<int> size = EntityField<int>("size", "ts_sqlservertest3");

    protected:
        void reset(const QString& tbName) {
            id = EntityField<qint64>("id", tbName);
            tbi1 = EntityField<qint64>("tbi1", tbName);
            tbi2 = EntityField<qint64>("tbi2", tbName);
            name = EntityField<QString>("name", tbName);
            size = EntityField<int>("size", tbName);
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
            return QStringLiteral("ts_sqlservertest3");
        }

        static QString getSourceName() {
            return getTableName();
        }

        static QStringList getFields() {
            return QStringList()
                << "id"
                << "tbi1"
                << "tbi2"
                << "name"
                << "size";
        }

        static QStringList getFieldsWithoutAutoIncrement() {
            return QStringList()
                << "tbi1"
                << "tbi2"
                << "name"
                << "size";
        }

        static QStringList getFieldsWithoutTimestamp() {
            return QStringList()
                << "id"
                << "tbi1"
                << "tbi2"
                << "name"
                << "size";
        }

        static QStringList getFieldsType() {
            return QStringList() 
                << QStringLiteral("id bigint primary key identity(1,1)")
                << QStringLiteral("tbi1 bigint null default -1")
                << QStringLiteral("tbi2 bigint null default -1")
                << QStringLiteral("name varchar(max)")
                << QStringLiteral("size int");
        }

        static QStringList getPrimaryKeys() {
            return QStringList() << "id";
        }

        static QList<QStringList> getClusteredIndexFields() {
            return QList<QStringList>();
        }

        static QList<QStringList> getUniqueClusteredIndexFields() {
            return QList<QStringList>();
        }

        static QList<QStringList> getNonClusteredIndexFields() {
            return QList<QStringList>();
        }

        static QList<QStringList> getUniqueNonClusteredIndexFields() {
            return QList<QStringList>();
        }

        static QString getIndexOption(const QString& name) {
            Q_UNUSED(name);
            return QString();
        }

        static bool isAutoIncrement(const QString& name) {
            return name == "id";
        }
    };

    struct Tool {
        static QVariantList getValueWithoutAutoIncrement(const SqlServerTest3& entity) {
            return QVariantList()
                << entity.tbi1
                << entity.tbi2
                << entity.name
                << entity.size;
        }

        static QVariant getValueByName(const SqlServerTest3& entity, const QString& target) {
            if (target == "id") {
                return entity.id;
            }
            if (target == "tbi1") {
                return entity.tbi1;
            }
            if (target == "tbi2") {
                return entity.tbi2;
            }
            if (target == "name") {
                return entity.name;
            }
            if (target == "size") {
                return entity.size;
            }
            return entity.__extra.value(target);
        }

        static void bindAutoIncrementId(SqlServerTest3& entity, const QVariant& id) {
            entity.id = id.value<qint64>();
        }

        static void bindValue(SqlServerTest3& entity, const QString& target, QVariant value) {
            if (target == "id") {
                entity.id = value.value<qint64>();
            } else if (target == "tbi1") {
                entity.tbi1 = value.value<qint64>();
            } else if (target == "tbi2") {
                entity.tbi2 = value.value<qint64>();
            } else if (target == "name") {
                entity.name = value.value<QString>();
            } else if (target == "size") {
                entity.size = value.value<int>();
            } else {
                entity.__putExtra(target, value);
            }
        }

        static SqlServerTest3 fromJson(const QJsonObject& object) {
            SqlServerTest3 entity;
            entity.id = object.value("id").toVariant().value<qint64>();
            entity.tbi1 = object.value("tbi1").toVariant().value<qint64>();
            entity.tbi2 = object.value("tbi2").toVariant().value<qint64>();
            entity.name = object.value("name").toVariant().value<QString>();
            entity.size = object.value("size").toVariant().value<int>();
            return entity;
        }

        static QJsonObject toJson(const SqlServerTest3& entity, QStringList excludeKeys = QStringList()) {
            QJsonObject object;
            object.insert("id", entity.id);
            object.insert("tbi1", entity.tbi1);
            object.insert("tbi2", entity.tbi2);
            object.insert("name", entity.name);
            object.insert("size", entity.size);

            for (const auto& key : excludeKeys) {
                object.remove(key);
            }
            return object;
        }
    };

public:
    //
    inline void setId(const qint64& id) {this->id = id;}
    //
    inline qint64 getId() const {return id;}
    //set 绑定到SqlServerTest1 id
    inline void setTbi1(const qint64& tbi1) {this->tbi1 = tbi1;}
    //get 绑定到SqlServerTest1 id
    inline qint64 getTbi1() const {return tbi1;}
    //set 绑定到SqlServerTest2 id
    inline void setTbi2(const qint64& tbi2) {this->tbi2 = tbi2;}
    //get 绑定到SqlServerTest2 id
    inline qint64 getTbi2() const {return tbi2;}
    //
    inline void setName(const QString& name) {this->name = name;}
    //
    inline QString getName() const {return name;}
    //
    inline void setSize(const int& size) {this->size = size;}
    //
    inline int getSize() const {return size;}
    //set temp data
    inline void __putExtra(const QString& key, const QVariant& extra) {this->__extra.insert(key, extra);}
    //get function select result, like get "as" field result
    inline QVariant __getExtra(const QString& key) const {return __extra.value(key);}
};
typedef QList<SqlServerTest3> SqlServerTest3List;
Q_DECLARE_METATYPE(SqlServerTest3);
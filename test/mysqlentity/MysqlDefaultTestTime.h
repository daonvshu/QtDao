﻿/*This file is auto generated by DbEntityGenerator*/
#pragma once

#include <qobject.h>
#include <qvariant.h>
#include <qdatetime.h>
#include <qjsonobject.h>

#include "condition/EntityField.h"

class MysqlDefaultTestTime {
private:
    //
    QString typetime;
    //
    QString typetime2;
    //
    QString typetime3;
    //
    QString typetime4;
    //
    QDate typedate;
    //
    QDate typedate2;
    //
    QDate typedate3;
    //
    QDate typedate4;
    //
    QDateTime typedatetime;
    //
    QDateTime typedatetime2;
    //
    QDateTime typedatetime3;
    //
    QDateTime typedatetime4;
    //
    QDateTime typedatetime5;
    //
    QDateTime typedatetime6;
    //
    QDateTime typedatetime7;
    //
    QDateTime typedatetime8;
    //
    QDateTime typetimestamp;
    //
    QDateTime typetimestamp2;
    //
    QDateTime typetimestamp3;
    //
    QDateTime typetimestamp4;
    //
    QDateTime typetimestamp5;
    //
    QDateTime typetimestamp6;
    //
    QDateTime typetimestamp7;
    //
    QDateTime typetimestamp8;

    QHash<QString, QVariant> __extra;

public:

    MysqlDefaultTestTime() {
        typetime = "120:59:59";
        typetime2 = QTime::currentTime().toString("HH:mm:ss");
        typetime3 = QTime::currentTime().toString("HH:mm:ss");
        typetime4 = QString();
        typedate = QDate::fromString("2020-01-01");
        typedate2 = QDate::currentDate();
        typedate3 = QDate::currentDate();
        typedate4 = QDate();
        typedatetime = QDateTime::fromString("2020-01-01 12:59:59");
        typedatetime2 = QDateTime::currentDateTime();
        typedatetime3 = QDateTime();
        typedatetime4 = QDateTime::currentDateTime();
        typedatetime5 = QDateTime::currentDateTime();
        typedatetime6 = QDateTime::currentDateTime();
        typedatetime7 = QDateTime::currentDateTime();
        typedatetime8 = QDateTime::fromString("null ON UPDATE CURRENT_TIMESTAMP");
        typetimestamp = QDateTime::fromString("2020-01-01 12:59:59");
        typetimestamp2 = QDateTime::currentDateTime();
        typetimestamp3 = QDateTime();
        typetimestamp4 = QDateTime::currentDateTime();
        typetimestamp5 = QDateTime::currentDateTime();
        typetimestamp6 = QDateTime::currentDateTime();
        typetimestamp7 = QDateTime::currentDateTime();
        typetimestamp8 = QDateTime::fromString("null ON UPDATE CURRENT_TIMESTAMP");
    }

    MysqlDefaultTestTime(
        const QString& typetime,
        const QString& typetime2,
        const QString& typetime3,
        const QString& typetime4,
        const QDate& typedate,
        const QDate& typedate2,
        const QDate& typedate3,
        const QDate& typedate4,
        const QDateTime& typedatetime,
        const QDateTime& typedatetime2,
        const QDateTime& typedatetime3,
        const QDateTime& typedatetime4,
        const QDateTime& typedatetime5,
        const QDateTime& typedatetime6,
        const QDateTime& typedatetime7,
        const QDateTime& typedatetime8,
        const QDateTime& typetimestamp,
        const QDateTime& typetimestamp2,
        const QDateTime& typetimestamp3,
        const QDateTime& typetimestamp4,
        const QDateTime& typetimestamp5,
        const QDateTime& typetimestamp6,
        const QDateTime& typetimestamp7,
        const QDateTime& typetimestamp8
    ) : typetime(typetime)
    , typetime2(typetime2)
    , typetime3(typetime3)
    , typetime4(typetime4)
    , typedate(typedate)
    , typedate2(typedate2)
    , typedate3(typedate3)
    , typedate4(typedate4)
    , typedatetime(typedatetime)
    , typedatetime2(typedatetime2)
    , typedatetime3(typedatetime3)
    , typedatetime4(typedatetime4)
    , typedatetime5(typedatetime5)
    , typedatetime6(typedatetime6)
    , typedatetime7(typedatetime7)
    , typedatetime8(typedatetime8)
    , typetimestamp(typetimestamp)
    , typetimestamp2(typetimestamp2)
    , typetimestamp3(typetimestamp3)
    , typetimestamp4(typetimestamp4)
    , typetimestamp5(typetimestamp5)
    , typetimestamp6(typetimestamp6)
    , typetimestamp7(typetimestamp7)
    , typetimestamp8(typetimestamp8)
    {
    }

public:
    class Fields {
    public:
        EntityField<QString> typetime = EntityField<QString>("typetime", "ts_mysqldefaulttesttime");
        EntityField<QString> typetime2 = EntityField<QString>("typetime2", "ts_mysqldefaulttesttime");
        EntityField<QString> typetime3 = EntityField<QString>("typetime3", "ts_mysqldefaulttesttime");
        EntityField<QString> typetime4 = EntityField<QString>("typetime4", "ts_mysqldefaulttesttime");
        EntityField<QDate> typedate = EntityField<QDate>("typedate", "ts_mysqldefaulttesttime");
        EntityField<QDate> typedate2 = EntityField<QDate>("typedate2", "ts_mysqldefaulttesttime");
        EntityField<QDate> typedate3 = EntityField<QDate>("typedate3", "ts_mysqldefaulttesttime");
        EntityField<QDate> typedate4 = EntityField<QDate>("typedate4", "ts_mysqldefaulttesttime");
        EntityField<QDateTime> typedatetime = EntityField<QDateTime>("typedatetime", "ts_mysqldefaulttesttime");
        EntityField<QDateTime> typedatetime2 = EntityField<QDateTime>("typedatetime2", "ts_mysqldefaulttesttime");
        EntityField<QDateTime> typedatetime3 = EntityField<QDateTime>("typedatetime3", "ts_mysqldefaulttesttime");
        EntityField<QDateTime> typedatetime4 = EntityField<QDateTime>("typedatetime4", "ts_mysqldefaulttesttime");
        EntityField<QDateTime> typedatetime5 = EntityField<QDateTime>("typedatetime5", "ts_mysqldefaulttesttime");
        EntityField<QDateTime> typedatetime6 = EntityField<QDateTime>("typedatetime6", "ts_mysqldefaulttesttime");
        EntityField<QDateTime> typedatetime7 = EntityField<QDateTime>("typedatetime7", "ts_mysqldefaulttesttime");
        EntityField<QDateTime> typedatetime8 = EntityField<QDateTime>("typedatetime8", "ts_mysqldefaulttesttime");
        EntityField<QDateTime> typetimestamp = EntityField<QDateTime>("typetimestamp", "ts_mysqldefaulttesttime");
        EntityField<QDateTime> typetimestamp2 = EntityField<QDateTime>("typetimestamp2", "ts_mysqldefaulttesttime");
        EntityField<QDateTime> typetimestamp3 = EntityField<QDateTime>("typetimestamp3", "ts_mysqldefaulttesttime");
        EntityField<QDateTime> typetimestamp4 = EntityField<QDateTime>("typetimestamp4", "ts_mysqldefaulttesttime");
        EntityField<QDateTime> typetimestamp5 = EntityField<QDateTime>("typetimestamp5", "ts_mysqldefaulttesttime");
        EntityField<QDateTime> typetimestamp6 = EntityField<QDateTime>("typetimestamp6", "ts_mysqldefaulttesttime");
        EntityField<QDateTime> typetimestamp7 = EntityField<QDateTime>("typetimestamp7", "ts_mysqldefaulttesttime");
        EntityField<QDateTime> typetimestamp8 = EntityField<QDateTime>("typetimestamp8", "ts_mysqldefaulttesttime");

    protected:
        void reset(const QString& tbName) {
            typetime = EntityField<QString>("typetime", tbName);
            typetime2 = EntityField<QString>("typetime2", tbName);
            typetime3 = EntityField<QString>("typetime3", tbName);
            typetime4 = EntityField<QString>("typetime4", tbName);
            typedate = EntityField<QDate>("typedate", tbName);
            typedate2 = EntityField<QDate>("typedate2", tbName);
            typedate3 = EntityField<QDate>("typedate3", tbName);
            typedate4 = EntityField<QDate>("typedate4", tbName);
            typedatetime = EntityField<QDateTime>("typedatetime", tbName);
            typedatetime2 = EntityField<QDateTime>("typedatetime2", tbName);
            typedatetime3 = EntityField<QDateTime>("typedatetime3", tbName);
            typedatetime4 = EntityField<QDateTime>("typedatetime4", tbName);
            typedatetime5 = EntityField<QDateTime>("typedatetime5", tbName);
            typedatetime6 = EntityField<QDateTime>("typedatetime6", tbName);
            typedatetime7 = EntityField<QDateTime>("typedatetime7", tbName);
            typedatetime8 = EntityField<QDateTime>("typedatetime8", tbName);
            typetimestamp = EntityField<QDateTime>("typetimestamp", tbName);
            typetimestamp2 = EntityField<QDateTime>("typetimestamp2", tbName);
            typetimestamp3 = EntityField<QDateTime>("typetimestamp3", tbName);
            typetimestamp4 = EntityField<QDateTime>("typetimestamp4", tbName);
            typetimestamp5 = EntityField<QDateTime>("typetimestamp5", tbName);
            typetimestamp6 = EntityField<QDateTime>("typetimestamp6", tbName);
            typetimestamp7 = EntityField<QDateTime>("typetimestamp7", tbName);
            typetimestamp8 = EntityField<QDateTime>("typetimestamp8", tbName);
        }
    };

    struct Info {
        enum {
            Attach = 1
        };

        static int fieldSize() {
            return 24;
        }

        static QString getTableName() {
            return QStringLiteral("ts_mysqldefaulttesttime");
        }

        static QString getSourceName() {
            return getTableName();
        }

        static QString getTableEngine() {
            return QString();
        }

        static QStringList getFields() {
            return QStringList()
                << "typetime"
                << "typetime2"
                << "typetime3"
                << "typetime4"
                << "typedate"
                << "typedate2"
                << "typedate3"
                << "typedate4"
                << "typedatetime"
                << "typedatetime2"
                << "typedatetime3"
                << "typedatetime4"
                << "typedatetime5"
                << "typedatetime6"
                << "typedatetime7"
                << "typedatetime8"
                << "typetimestamp"
                << "typetimestamp2"
                << "typetimestamp3"
                << "typetimestamp4"
                << "typetimestamp5"
                << "typetimestamp6"
                << "typetimestamp7"
                << "typetimestamp8";
        }

        static QStringList getFieldsWithoutAutoIncrement() {
            return QStringList()
                << "typetime"
                << "typetime2"
                << "typetime3"
                << "typetime4"
                << "typedate"
                << "typedate2"
                << "typedate3"
                << "typedate4"
                << "typedatetime"
                << "typedatetime2"
                << "typedatetime3"
                << "typedatetime4"
                << "typedatetime5"
                << "typedatetime6"
                << "typedatetime7"
                << "typedatetime8"
                << "typetimestamp"
                << "typetimestamp2"
                << "typetimestamp3"
                << "typetimestamp4"
                << "typetimestamp5"
                << "typetimestamp6"
                << "typetimestamp7"
                << "typetimestamp8";
        }

        static QStringList getFieldsType() {
            return QStringList() 
                << QStringLiteral("typetime time null default '120:59:59' comment ''")
                << QStringLiteral("typetime2 time null default null comment ''")
                << QStringLiteral("typetime3 time null default null comment ''")
                << QStringLiteral("typetime4 time null default null comment ''")
                << QStringLiteral("typedate date null default '2020-01-01' comment ''")
                << QStringLiteral("typedate2 date null default null comment ''")
                << QStringLiteral("typedate3 date null default null comment ''")
                << QStringLiteral("typedate4 date null default null comment ''")
                << QStringLiteral("typedatetime datetime null default '2020-01-01 12:59:59' comment ''")
                << QStringLiteral("typedatetime2 datetime null default null comment ''")
                << QStringLiteral("typedatetime3 datetime null default null comment ''")
                << QStringLiteral("typedatetime4 datetime null default CURRENT_TIMESTAMP comment ''")
                << QStringLiteral("typedatetime5 datetime null default CURRENT_TIMESTAMP comment ''")
                << QStringLiteral("typedatetime6 datetime(3) null default CURRENT_TIMESTAMP(3) comment ''")
                << QStringLiteral("typedatetime7 datetime(3) null default CURRENT_TIMESTAMP(3) ON UPDATE CURRENT_TIMESTAMP(3) comment ''")
                << QStringLiteral("typedatetime8 datetime null default null ON UPDATE CURRENT_TIMESTAMP comment ''")
                << QStringLiteral("typetimestamp timestamp null default '2020-01-01 12:59:59' comment ''")
                << QStringLiteral("typetimestamp2 timestamp null default null comment ''")
                << QStringLiteral("typetimestamp3 timestamp null default null comment ''")
                << QStringLiteral("typetimestamp4 timestamp null default CURRENT_TIMESTAMP comment ''")
                << QStringLiteral("typetimestamp5 timestamp null default CURRENT_TIMESTAMP comment ''")
                << QStringLiteral("typetimestamp6 timestamp(3) null default CURRENT_TIMESTAMP(3) comment ''")
                << QStringLiteral("typetimestamp7 timestamp(3) null default CURRENT_TIMESTAMP(3) ON UPDATE CURRENT_TIMESTAMP(3) comment ''")
                << QStringLiteral("typetimestamp8 timestamp null default null ON UPDATE CURRENT_TIMESTAMP comment ''");
        }

        static QStringList getPrimaryKeys() {
            return QStringList();
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
        static QVariantList getValueWithoutAutoIncrement(const MysqlDefaultTestTime& entity) {
            return QVariantList()
                << entity.typetime
                << entity.typetime2
                << entity.typetime3
                << entity.typetime4
                << entity.typedate
                << entity.typedate2
                << entity.typedate3
                << entity.typedate4
                << entity.typedatetime
                << entity.typedatetime2
                << entity.typedatetime3
                << entity.typedatetime4
                << entity.typedatetime5
                << entity.typedatetime6
                << entity.typedatetime7
                << entity.typedatetime8
                << entity.typetimestamp
                << entity.typetimestamp2
                << entity.typetimestamp3
                << entity.typetimestamp4
                << entity.typetimestamp5
                << entity.typetimestamp6
                << entity.typetimestamp7
                << entity.typetimestamp8;
        }

        static QVariant getValueByName(const MysqlDefaultTestTime& entity, const QString& target) {
            if (target == "typetime") {
                return entity.typetime;
            }
            if (target == "typetime2") {
                return entity.typetime2;
            }
            if (target == "typetime3") {
                return entity.typetime3;
            }
            if (target == "typetime4") {
                return entity.typetime4;
            }
            if (target == "typedate") {
                return entity.typedate;
            }
            if (target == "typedate2") {
                return entity.typedate2;
            }
            if (target == "typedate3") {
                return entity.typedate3;
            }
            if (target == "typedate4") {
                return entity.typedate4;
            }
            if (target == "typedatetime") {
                return entity.typedatetime;
            }
            if (target == "typedatetime2") {
                return entity.typedatetime2;
            }
            if (target == "typedatetime3") {
                return entity.typedatetime3;
            }
            if (target == "typedatetime4") {
                return entity.typedatetime4;
            }
            if (target == "typedatetime5") {
                return entity.typedatetime5;
            }
            if (target == "typedatetime6") {
                return entity.typedatetime6;
            }
            if (target == "typedatetime7") {
                return entity.typedatetime7;
            }
            if (target == "typedatetime8") {
                return entity.typedatetime8;
            }
            if (target == "typetimestamp") {
                return entity.typetimestamp;
            }
            if (target == "typetimestamp2") {
                return entity.typetimestamp2;
            }
            if (target == "typetimestamp3") {
                return entity.typetimestamp3;
            }
            if (target == "typetimestamp4") {
                return entity.typetimestamp4;
            }
            if (target == "typetimestamp5") {
                return entity.typetimestamp5;
            }
            if (target == "typetimestamp6") {
                return entity.typetimestamp6;
            }
            if (target == "typetimestamp7") {
                return entity.typetimestamp7;
            }
            if (target == "typetimestamp8") {
                return entity.typetimestamp8;
            }
            return entity.__extra.value(target);
        }

        static void bindAutoIncrementId(MysqlDefaultTestTime& entity, const QVariant& id) {
            Q_UNUSED(entity);
            Q_UNUSED(id);
        }

        static void bindValue(MysqlDefaultTestTime& entity, const QString& target, QVariant value) {
            if (target == "typetime") {
                entity.typetime = value.value<QString>();
            } else if (target == "typetime2") {
                entity.typetime2 = value.value<QString>();
            } else if (target == "typetime3") {
                entity.typetime3 = value.value<QString>();
            } else if (target == "typetime4") {
                entity.typetime4 = value.value<QString>();
            } else if (target == "typedate") {
                entity.typedate = value.value<QDate>();
            } else if (target == "typedate2") {
                entity.typedate2 = value.value<QDate>();
            } else if (target == "typedate3") {
                entity.typedate3 = value.value<QDate>();
            } else if (target == "typedate4") {
                entity.typedate4 = value.value<QDate>();
            } else if (target == "typedatetime") {
                entity.typedatetime = value.value<QDateTime>();
            } else if (target == "typedatetime2") {
                entity.typedatetime2 = value.value<QDateTime>();
            } else if (target == "typedatetime3") {
                entity.typedatetime3 = value.value<QDateTime>();
            } else if (target == "typedatetime4") {
                entity.typedatetime4 = value.value<QDateTime>();
            } else if (target == "typedatetime5") {
                entity.typedatetime5 = value.value<QDateTime>();
            } else if (target == "typedatetime6") {
                entity.typedatetime6 = value.value<QDateTime>();
            } else if (target == "typedatetime7") {
                entity.typedatetime7 = value.value<QDateTime>();
            } else if (target == "typedatetime8") {
                entity.typedatetime8 = value.value<QDateTime>();
            } else if (target == "typetimestamp") {
                entity.typetimestamp = value.value<QDateTime>();
            } else if (target == "typetimestamp2") {
                entity.typetimestamp2 = value.value<QDateTime>();
            } else if (target == "typetimestamp3") {
                entity.typetimestamp3 = value.value<QDateTime>();
            } else if (target == "typetimestamp4") {
                entity.typetimestamp4 = value.value<QDateTime>();
            } else if (target == "typetimestamp5") {
                entity.typetimestamp5 = value.value<QDateTime>();
            } else if (target == "typetimestamp6") {
                entity.typetimestamp6 = value.value<QDateTime>();
            } else if (target == "typetimestamp7") {
                entity.typetimestamp7 = value.value<QDateTime>();
            } else if (target == "typetimestamp8") {
                entity.typetimestamp8 = value.value<QDateTime>();
            } else {
                entity.__putExtra(target, value);
            }
        }

        static MysqlDefaultTestTime fromJson(const QJsonObject& object) {
            MysqlDefaultTestTime entity;
            entity.typetime = object.value("typetime").toVariant().value<QString>();
            entity.typetime2 = object.value("typetime2").toVariant().value<QString>();
            entity.typetime3 = object.value("typetime3").toVariant().value<QString>();
            entity.typetime4 = object.value("typetime4").toVariant().value<QString>();
            entity.typedate = QDate::fromString(object.value("typedate").toString(), "yyyy-MM-dd");
            entity.typedate2 = QDate::fromString(object.value("typedate2").toString(), "yyyy-MM-dd");
            entity.typedate3 = QDate::fromString(object.value("typedate3").toString(), "yyyy-MM-dd");
            entity.typedate4 = QDate::fromString(object.value("typedate4").toString(), "yyyy-MM-dd");
            entity.typedatetime = QDateTime::fromString(object.value("typedatetime").toString(), "yyyy-MM-dd HH:mm:ss");
            entity.typedatetime2 = QDateTime::fromString(object.value("typedatetime2").toString(), "yyyy-MM-dd HH:mm:ss");
            entity.typedatetime3 = QDateTime::fromString(object.value("typedatetime3").toString(), "yyyy-MM-dd HH:mm:ss");
            entity.typedatetime4 = QDateTime::fromString(object.value("typedatetime4").toString(), "yyyy-MM-dd HH:mm:ss");
            entity.typedatetime5 = QDateTime::fromString(object.value("typedatetime5").toString(), "yyyy-MM-dd HH:mm:ss");
            entity.typedatetime6 = QDateTime::fromString(object.value("typedatetime6").toString(), "yyyy-MM-dd HH:mm:ss");
            entity.typedatetime7 = QDateTime::fromString(object.value("typedatetime7").toString(), "yyyy-MM-dd HH:mm:ss");
            entity.typedatetime8 = QDateTime::fromString(object.value("typedatetime8").toString(), "yyyy-MM-dd HH:mm:ss");
            entity.typetimestamp = QDateTime::fromString(object.value("typetimestamp").toString(), "yyyy-MM-dd HH:mm:ss");
            entity.typetimestamp2 = QDateTime::fromString(object.value("typetimestamp2").toString(), "yyyy-MM-dd HH:mm:ss");
            entity.typetimestamp3 = QDateTime::fromString(object.value("typetimestamp3").toString(), "yyyy-MM-dd HH:mm:ss");
            entity.typetimestamp4 = QDateTime::fromString(object.value("typetimestamp4").toString(), "yyyy-MM-dd HH:mm:ss");
            entity.typetimestamp5 = QDateTime::fromString(object.value("typetimestamp5").toString(), "yyyy-MM-dd HH:mm:ss");
            entity.typetimestamp6 = QDateTime::fromString(object.value("typetimestamp6").toString(), "yyyy-MM-dd HH:mm:ss");
            entity.typetimestamp7 = QDateTime::fromString(object.value("typetimestamp7").toString(), "yyyy-MM-dd HH:mm:ss");
            entity.typetimestamp8 = QDateTime::fromString(object.value("typetimestamp8").toString(), "yyyy-MM-dd HH:mm:ss");
            return entity;
        }

        static QJsonObject toJson(const MysqlDefaultTestTime& entity, QStringList excludeKeys = QStringList()) {
            QJsonObject object;
            object.insert("typetime", entity.typetime);
            object.insert("typetime2", entity.typetime2);
            object.insert("typetime3", entity.typetime3);
            object.insert("typetime4", entity.typetime4);
            object.insert("typedate", entity.typedate.toString("yyyy-MM-dd"));
            object.insert("typedate2", entity.typedate2.toString("yyyy-MM-dd"));
            object.insert("typedate3", entity.typedate3.toString("yyyy-MM-dd"));
            object.insert("typedate4", entity.typedate4.toString("yyyy-MM-dd"));
            object.insert("typedatetime", entity.typedatetime.toString("yyyy-MM-dd HH:mm:ss"));
            object.insert("typedatetime2", entity.typedatetime2.toString("yyyy-MM-dd HH:mm:ss"));
            object.insert("typedatetime3", entity.typedatetime3.toString("yyyy-MM-dd HH:mm:ss"));
            object.insert("typedatetime4", entity.typedatetime4.toString("yyyy-MM-dd HH:mm:ss"));
            object.insert("typedatetime5", entity.typedatetime5.toString("yyyy-MM-dd HH:mm:ss"));
            object.insert("typedatetime6", entity.typedatetime6.toString("yyyy-MM-dd HH:mm:ss"));
            object.insert("typedatetime7", entity.typedatetime7.toString("yyyy-MM-dd HH:mm:ss"));
            object.insert("typedatetime8", entity.typedatetime8.toString("yyyy-MM-dd HH:mm:ss"));
            object.insert("typetimestamp", entity.typetimestamp.toString("yyyy-MM-dd HH:mm:ss"));
            object.insert("typetimestamp2", entity.typetimestamp2.toString("yyyy-MM-dd HH:mm:ss"));
            object.insert("typetimestamp3", entity.typetimestamp3.toString("yyyy-MM-dd HH:mm:ss"));
            object.insert("typetimestamp4", entity.typetimestamp4.toString("yyyy-MM-dd HH:mm:ss"));
            object.insert("typetimestamp5", entity.typetimestamp5.toString("yyyy-MM-dd HH:mm:ss"));
            object.insert("typetimestamp6", entity.typetimestamp6.toString("yyyy-MM-dd HH:mm:ss"));
            object.insert("typetimestamp7", entity.typetimestamp7.toString("yyyy-MM-dd HH:mm:ss"));
            object.insert("typetimestamp8", entity.typetimestamp8.toString("yyyy-MM-dd HH:mm:ss"));

            for (const auto& key : excludeKeys) {
                object.remove(key);
            }
            return object;
        }
    };

public:
    //
    inline void setTypetime(const QString& typetime) {this->typetime = typetime;}
    //
    inline QString getTypetime() const {return typetime;}
    //
    inline void setTypetime2(const QString& typetime2) {this->typetime2 = typetime2;}
    //
    inline QString getTypetime2() const {return typetime2;}
    //
    inline void setTypetime3(const QString& typetime3) {this->typetime3 = typetime3;}
    //
    inline QString getTypetime3() const {return typetime3;}
    //
    inline void setTypetime4(const QString& typetime4) {this->typetime4 = typetime4;}
    //
    inline QString getTypetime4() const {return typetime4;}
    //
    inline void setTypedate(const QDate& typedate) {this->typedate = typedate;}
    //
    inline QDate getTypedate() const {return typedate;}
    //
    inline void setTypedate2(const QDate& typedate2) {this->typedate2 = typedate2;}
    //
    inline QDate getTypedate2() const {return typedate2;}
    //
    inline void setTypedate3(const QDate& typedate3) {this->typedate3 = typedate3;}
    //
    inline QDate getTypedate3() const {return typedate3;}
    //
    inline void setTypedate4(const QDate& typedate4) {this->typedate4 = typedate4;}
    //
    inline QDate getTypedate4() const {return typedate4;}
    //
    inline void setTypedatetime(const QDateTime& typedatetime) {this->typedatetime = typedatetime;}
    //
    inline QDateTime getTypedatetime() const {return typedatetime;}
    //
    inline void setTypedatetime2(const QDateTime& typedatetime2) {this->typedatetime2 = typedatetime2;}
    //
    inline QDateTime getTypedatetime2() const {return typedatetime2;}
    //
    inline void setTypedatetime3(const QDateTime& typedatetime3) {this->typedatetime3 = typedatetime3;}
    //
    inline QDateTime getTypedatetime3() const {return typedatetime3;}
    //
    inline void setTypedatetime4(const QDateTime& typedatetime4) {this->typedatetime4 = typedatetime4;}
    //
    inline QDateTime getTypedatetime4() const {return typedatetime4;}
    //
    inline void setTypedatetime5(const QDateTime& typedatetime5) {this->typedatetime5 = typedatetime5;}
    //
    inline QDateTime getTypedatetime5() const {return typedatetime5;}
    //
    inline void setTypedatetime6(const QDateTime& typedatetime6) {this->typedatetime6 = typedatetime6;}
    //
    inline QDateTime getTypedatetime6() const {return typedatetime6;}
    //
    inline void setTypedatetime7(const QDateTime& typedatetime7) {this->typedatetime7 = typedatetime7;}
    //
    inline QDateTime getTypedatetime7() const {return typedatetime7;}
    //
    inline void setTypedatetime8(const QDateTime& typedatetime8) {this->typedatetime8 = typedatetime8;}
    //
    inline QDateTime getTypedatetime8() const {return typedatetime8;}
    //
    inline void setTypetimestamp(const QDateTime& typetimestamp) {this->typetimestamp = typetimestamp;}
    //
    inline QDateTime getTypetimestamp() const {return typetimestamp;}
    //
    inline void setTypetimestamp2(const QDateTime& typetimestamp2) {this->typetimestamp2 = typetimestamp2;}
    //
    inline QDateTime getTypetimestamp2() const {return typetimestamp2;}
    //
    inline void setTypetimestamp3(const QDateTime& typetimestamp3) {this->typetimestamp3 = typetimestamp3;}
    //
    inline QDateTime getTypetimestamp3() const {return typetimestamp3;}
    //
    inline void setTypetimestamp4(const QDateTime& typetimestamp4) {this->typetimestamp4 = typetimestamp4;}
    //
    inline QDateTime getTypetimestamp4() const {return typetimestamp4;}
    //
    inline void setTypetimestamp5(const QDateTime& typetimestamp5) {this->typetimestamp5 = typetimestamp5;}
    //
    inline QDateTime getTypetimestamp5() const {return typetimestamp5;}
    //
    inline void setTypetimestamp6(const QDateTime& typetimestamp6) {this->typetimestamp6 = typetimestamp6;}
    //
    inline QDateTime getTypetimestamp6() const {return typetimestamp6;}
    //
    inline void setTypetimestamp7(const QDateTime& typetimestamp7) {this->typetimestamp7 = typetimestamp7;}
    //
    inline QDateTime getTypetimestamp7() const {return typetimestamp7;}
    //
    inline void setTypetimestamp8(const QDateTime& typetimestamp8) {this->typetimestamp8 = typetimestamp8;}
    //
    inline QDateTime getTypetimestamp8() const {return typetimestamp8;}
    //set temp data
    inline void __putExtra(const QString& key, const QVariant& extra) {this->__extra.insert(key, extra);}
    //get function select result, like get "as" field result
    inline QVariant __getExtra(const QString& key) const {return __extra.value(key);}
};
typedef QList<MysqlDefaultTestTime> MysqlDefaultTestTimeList;
Q_DECLARE_METATYPE(MysqlDefaultTestTime);
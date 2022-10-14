﻿/*This file is auto generated by vscode-qtdao*/
#pragma once

#include <qobject.h>
#include <qvariant.h>
#include <qdatetime.h>
#include <qjsonobject.h>

#include "condition/entityfield.h"

class SqlServerDefaultTestBytes {
    Q_GADGET

    Q_PROPERTY(bool typebit MEMBER typebit)
    Q_PROPERTY(bool typebitnull MEMBER typebitnull)
    Q_PROPERTY(QByteArray typebinary MEMBER typebinary)
    Q_PROPERTY(QByteArray typebinary2 MEMBER typebinary2)
    Q_PROPERTY(QByteArray typebinary3 MEMBER typebinary3)
    Q_PROPERTY(QByteArray typebinary4 MEMBER typebinary4)
    Q_PROPERTY(QByteArray typebinary5 MEMBER typebinary5)
    Q_PROPERTY(QByteArray typevarbinary MEMBER typevarbinary)
    Q_PROPERTY(QByteArray typevarbinary2 MEMBER typevarbinary2)
    Q_PROPERTY(QByteArray typevarbinary3 MEMBER typevarbinary3)
    Q_PROPERTY(QByteArray typevarbinary4 MEMBER typevarbinary4)
    Q_PROPERTY(QByteArray typevarbinary5 MEMBER typevarbinary5)
    Q_PROPERTY(QByteArray typevarbinarymax MEMBER typevarbinarymax)
    Q_PROPERTY(QByteArray typevarbinarymax2 MEMBER typevarbinarymax2)
    Q_PROPERTY(QByteArray typevarbinarymax3 MEMBER typevarbinarymax3)
    Q_PROPERTY(QByteArray typevarbinarymax4 MEMBER typevarbinarymax4)
    Q_PROPERTY(QByteArray typevarbinarymax5 MEMBER typevarbinarymax5)
    Q_PROPERTY(QVariant typevariant MEMBER typevariant)
    Q_PROPERTY(QByteArray typeuniqueidentifier MEMBER typeuniqueidentifier)
    Q_PROPERTY(QByteArray typexml MEMBER typexml)
    Q_PROPERTY(QVariantMap extra MEMBER __extra)

private:
    //
    bool typebit;
    //
    bool typebitnull;
    //
    QByteArray typebinary;
    //
    QByteArray typebinary2;
    //
    QByteArray typebinary3;
    //
    QByteArray typebinary4;
    //
    QByteArray typebinary5;
    //
    QByteArray typevarbinary;
    //
    QByteArray typevarbinary2;
    //
    QByteArray typevarbinary3;
    //
    QByteArray typevarbinary4;
    //
    QByteArray typevarbinary5;
    //
    QByteArray typevarbinarymax;
    //
    QByteArray typevarbinarymax2;
    //
    QByteArray typevarbinarymax3;
    //
    QByteArray typevarbinarymax4;
    //
    QByteArray typevarbinarymax5;
    //
    QVariant typevariant;
    //
    QByteArray typeuniqueidentifier;
    //
    QByteArray typexml;

    QVariantMap __extra;

public:

   SqlServerDefaultTestBytes() {
        typebit = 0;
        typebitnull = bool();
        typebinary = "string";
        typebinary2 = QByteArray();
        typebinary3 = QByteArray();
        typebinary4 = "null";
        typebinary5 = QByteArray("string");
        typevarbinary = "string";
        typevarbinary2 = QByteArray();
        typevarbinary3 = QByteArray();
        typevarbinary4 = "null";
        typevarbinary5 = QByteArray("string");
        typevarbinarymax = "string";
        typevarbinarymax2 = QByteArray();
        typevarbinarymax3 = QByteArray();
        typevarbinarymax4 = "null";
        typevarbinarymax5 = QByteArray("string");
        typevariant = "null";
        typeuniqueidentifier = "null";
        typexml = "null";
   }

    SqlServerDefaultTestBytes(
        const bool& typebit,
        const bool& typebitnull,
        const QByteArray& typebinary,
        const QByteArray& typebinary2,
        const QByteArray& typebinary3,
        const QByteArray& typebinary4,
        const QByteArray& typebinary5,
        const QByteArray& typevarbinary,
        const QByteArray& typevarbinary2,
        const QByteArray& typevarbinary3,
        const QByteArray& typevarbinary4,
        const QByteArray& typevarbinary5,
        const QByteArray& typevarbinarymax,
        const QByteArray& typevarbinarymax2,
        const QByteArray& typevarbinarymax3,
        const QByteArray& typevarbinarymax4,
        const QByteArray& typevarbinarymax5,
        const QVariant& typevariant,
        const QByteArray& typeuniqueidentifier,
        const QByteArray& typexml
    ) : typebit(typebit)
    , typebitnull(typebitnull)
    , typebinary(typebinary)
    , typebinary2(typebinary2)
    , typebinary3(typebinary3)
    , typebinary4(typebinary4)
    , typebinary5(typebinary5)
    , typevarbinary(typevarbinary)
    , typevarbinary2(typevarbinary2)
    , typevarbinary3(typevarbinary3)
    , typevarbinary4(typevarbinary4)
    , typevarbinary5(typevarbinary5)
    , typevarbinarymax(typevarbinarymax)
    , typevarbinarymax2(typevarbinarymax2)
    , typevarbinarymax3(typevarbinarymax3)
    , typevarbinarymax4(typevarbinarymax4)
    , typevarbinarymax5(typevarbinarymax5)
    , typevariant(typevariant)
    , typeuniqueidentifier(typeuniqueidentifier)
    , typexml(typexml)
    {
    }

public:
    class Fields {
    public:
        dao::EntityField<bool> typebit = dao::EntityField<bool>("typebit", "ts_sqlserverdefaulttestbytes");
        dao::EntityField<bool> typebitnull = dao::EntityField<bool>("typebitnull", "ts_sqlserverdefaulttestbytes");
        dao::EntityField<QByteArray> typebinary = dao::EntityField<QByteArray>("typebinary", "ts_sqlserverdefaulttestbytes");
        dao::EntityField<QByteArray> typebinary2 = dao::EntityField<QByteArray>("typebinary2", "ts_sqlserverdefaulttestbytes");
        dao::EntityField<QByteArray> typebinary3 = dao::EntityField<QByteArray>("typebinary3", "ts_sqlserverdefaulttestbytes");
        dao::EntityField<QByteArray> typebinary4 = dao::EntityField<QByteArray>("typebinary4", "ts_sqlserverdefaulttestbytes");
        dao::EntityField<QByteArray> typebinary5 = dao::EntityField<QByteArray>("typebinary5", "ts_sqlserverdefaulttestbytes");
        dao::EntityField<QByteArray> typevarbinary = dao::EntityField<QByteArray>("typevarbinary", "ts_sqlserverdefaulttestbytes");
        dao::EntityField<QByteArray> typevarbinary2 = dao::EntityField<QByteArray>("typevarbinary2", "ts_sqlserverdefaulttestbytes");
        dao::EntityField<QByteArray> typevarbinary3 = dao::EntityField<QByteArray>("typevarbinary3", "ts_sqlserverdefaulttestbytes");
        dao::EntityField<QByteArray> typevarbinary4 = dao::EntityField<QByteArray>("typevarbinary4", "ts_sqlserverdefaulttestbytes");
        dao::EntityField<QByteArray> typevarbinary5 = dao::EntityField<QByteArray>("typevarbinary5", "ts_sqlserverdefaulttestbytes");
        dao::EntityField<QByteArray> typevarbinarymax = dao::EntityField<QByteArray>("typevarbinarymax", "ts_sqlserverdefaulttestbytes");
        dao::EntityField<QByteArray> typevarbinarymax2 = dao::EntityField<QByteArray>("typevarbinarymax2", "ts_sqlserverdefaulttestbytes");
        dao::EntityField<QByteArray> typevarbinarymax3 = dao::EntityField<QByteArray>("typevarbinarymax3", "ts_sqlserverdefaulttestbytes");
        dao::EntityField<QByteArray> typevarbinarymax4 = dao::EntityField<QByteArray>("typevarbinarymax4", "ts_sqlserverdefaulttestbytes");
        dao::EntityField<QByteArray> typevarbinarymax5 = dao::EntityField<QByteArray>("typevarbinarymax5", "ts_sqlserverdefaulttestbytes");
        dao::EntityField<QVariant> typevariant = dao::EntityField<QVariant>("typevariant", "ts_sqlserverdefaulttestbytes");
        dao::EntityField<QByteArray> typeuniqueidentifier = dao::EntityField<QByteArray>("typeuniqueidentifier", "ts_sqlserverdefaulttestbytes");
        dao::EntityField<QByteArray> typexml = dao::EntityField<QByteArray>("typexml", "ts_sqlserverdefaulttestbytes");

    protected:
        void reset(const QString& tbName) {
            typebit = dao::EntityField<bool>("typebit", tbName);
            typebitnull = dao::EntityField<bool>("typebitnull", tbName);
            typebinary = dao::EntityField<QByteArray>("typebinary", tbName);
            typebinary2 = dao::EntityField<QByteArray>("typebinary2", tbName);
            typebinary3 = dao::EntityField<QByteArray>("typebinary3", tbName);
            typebinary4 = dao::EntityField<QByteArray>("typebinary4", tbName);
            typebinary5 = dao::EntityField<QByteArray>("typebinary5", tbName);
            typevarbinary = dao::EntityField<QByteArray>("typevarbinary", tbName);
            typevarbinary2 = dao::EntityField<QByteArray>("typevarbinary2", tbName);
            typevarbinary3 = dao::EntityField<QByteArray>("typevarbinary3", tbName);
            typevarbinary4 = dao::EntityField<QByteArray>("typevarbinary4", tbName);
            typevarbinary5 = dao::EntityField<QByteArray>("typevarbinary5", tbName);
            typevarbinarymax = dao::EntityField<QByteArray>("typevarbinarymax", tbName);
            typevarbinarymax2 = dao::EntityField<QByteArray>("typevarbinarymax2", tbName);
            typevarbinarymax3 = dao::EntityField<QByteArray>("typevarbinarymax3", tbName);
            typevarbinarymax4 = dao::EntityField<QByteArray>("typevarbinarymax4", tbName);
            typevarbinarymax5 = dao::EntityField<QByteArray>("typevarbinarymax5", tbName);
            typevariant = dao::EntityField<QVariant>("typevariant", tbName);
            typeuniqueidentifier = dao::EntityField<QByteArray>("typeuniqueidentifier", tbName);
            typexml = dao::EntityField<QByteArray>("typexml", tbName);
        }
    };

    struct Info {
        enum {
            Attach = 1
        };

        static int fieldSize() {
            return 20;
        }

        static QString getTableName() {
            return QStringLiteral("ts_sqlserverdefaulttestbytes");
        }

        static QString getSourceName() {
            return getTableName();
        }

        static QStringList getFields() {
            return QStringList()
                << "typebit"
                << "typebitnull"
                << "typebinary"
                << "typebinary2"
                << "typebinary3"
                << "typebinary4"
                << "typebinary5"
                << "typevarbinary"
                << "typevarbinary2"
                << "typevarbinary3"
                << "typevarbinary4"
                << "typevarbinary5"
                << "typevarbinarymax"
                << "typevarbinarymax2"
                << "typevarbinarymax3"
                << "typevarbinarymax4"
                << "typevarbinarymax5"
                << "typevariant"
                << "typeuniqueidentifier"
                << "typexml";
        }

        static QStringList getFieldsWithoutAutoIncrement() {
            return QStringList()
                << "typebit"
                << "typebitnull"
                << "typebinary"
                << "typebinary2"
                << "typebinary3"
                << "typebinary4"
                << "typebinary5"
                << "typevarbinary"
                << "typevarbinary2"
                << "typevarbinary3"
                << "typevarbinary4"
                << "typevarbinary5"
                << "typevarbinarymax"
                << "typevarbinarymax2"
                << "typevarbinarymax3"
                << "typevarbinarymax4"
                << "typevarbinarymax5"
                << "typevariant"
                << "typeuniqueidentifier"
                << "typexml";
        }

        static QStringList getFieldsWithoutTimestamp() {
            return QStringList()
                << "typebit"
                << "typebitnull"
                << "typebinary"
                << "typebinary2"
                << "typebinary3"
                << "typebinary4"
                << "typebinary5"
                << "typevarbinary"
                << "typevarbinary2"
                << "typevarbinary3"
                << "typevarbinary4"
                << "typevarbinary5"
                << "typevarbinarymax"
                << "typevarbinarymax2"
                << "typevarbinarymax3"
                << "typevarbinarymax4"
                << "typevarbinarymax5"
                << "typevariant"
                << "typeuniqueidentifier"
                << "typexml";
        }

        static QStringList getFieldsType() {
            return QStringList() 
                << QStringLiteral("typebit bit null default 0")
                << QStringLiteral("typebitnull bit null default null")
                << QStringLiteral("typebinary binary null default null")
                << QStringLiteral("typebinary2 binary null default null")
                << QStringLiteral("typebinary3 binary null default null")
                << QStringLiteral("typebinary4 binary null default null")
                << QStringLiteral("typebinary5 binary null default null")
                << QStringLiteral("typevarbinary varbinary null default null")
                << QStringLiteral("typevarbinary2 varbinary null default null")
                << QStringLiteral("typevarbinary3 varbinary null default null")
                << QStringLiteral("typevarbinary4 varbinary null default null")
                << QStringLiteral("typevarbinary5 varbinary null default null")
                << QStringLiteral("typevarbinarymax varbinary(max) null default null")
                << QStringLiteral("typevarbinarymax2 varbinary(max) null default null")
                << QStringLiteral("typevarbinarymax3 varbinary(max) null default null")
                << QStringLiteral("typevarbinarymax4 varbinary(max) null default null")
                << QStringLiteral("typevarbinarymax5 varbinary(max) null default null")
                << QStringLiteral("typevariant sql_variant null default null")
                << QStringLiteral("typeuniqueidentifier uniqueidentifier null default null")
                << QStringLiteral("typexml xml null default null");
        }

        static QStringList getPrimaryKeys() {
            return QStringList();
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
            Q_UNUSED(name);
            return false;
        }
    };

    struct Tool {
        static QVariantList getValueWithoutAutoIncrement(const SqlServerDefaultTestBytes& entity) {
            return QVariantList()
                << entity.typebit
                << entity.typebitnull
                << entity.typebinary
                << entity.typebinary2
                << entity.typebinary3
                << entity.typebinary4
                << entity.typebinary5
                << entity.typevarbinary
                << entity.typevarbinary2
                << entity.typevarbinary3
                << entity.typevarbinary4
                << entity.typevarbinary5
                << entity.typevarbinarymax
                << entity.typevarbinarymax2
                << entity.typevarbinarymax3
                << entity.typevarbinarymax4
                << entity.typevarbinarymax5
                << entity.typevariant
                << entity.typeuniqueidentifier
                << entity.typexml;
        }

        static QVariant getValueByName(const SqlServerDefaultTestBytes& entity, const QString& target) {
            if (target == "typebit") {
                return entity.typebit;
            }
            if (target == "typebitnull") {
                return entity.typebitnull;
            }
            if (target == "typebinary") {
                return entity.typebinary;
            }
            if (target == "typebinary2") {
                return entity.typebinary2;
            }
            if (target == "typebinary3") {
                return entity.typebinary3;
            }
            if (target == "typebinary4") {
                return entity.typebinary4;
            }
            if (target == "typebinary5") {
                return entity.typebinary5;
            }
            if (target == "typevarbinary") {
                return entity.typevarbinary;
            }
            if (target == "typevarbinary2") {
                return entity.typevarbinary2;
            }
            if (target == "typevarbinary3") {
                return entity.typevarbinary3;
            }
            if (target == "typevarbinary4") {
                return entity.typevarbinary4;
            }
            if (target == "typevarbinary5") {
                return entity.typevarbinary5;
            }
            if (target == "typevarbinarymax") {
                return entity.typevarbinarymax;
            }
            if (target == "typevarbinarymax2") {
                return entity.typevarbinarymax2;
            }
            if (target == "typevarbinarymax3") {
                return entity.typevarbinarymax3;
            }
            if (target == "typevarbinarymax4") {
                return entity.typevarbinarymax4;
            }
            if (target == "typevarbinarymax5") {
                return entity.typevarbinarymax5;
            }
            if (target == "typevariant") {
                return entity.typevariant;
            }
            if (target == "typeuniqueidentifier") {
                return entity.typeuniqueidentifier;
            }
            if (target == "typexml") {
                return entity.typexml;
            }
            return entity.__extra.value(target);
        }

        static void bindAutoIncrementId(SqlServerDefaultTestBytes& entity, const QVariant& id) {
            Q_UNUSED(entity);
            Q_UNUSED(id);
        }

        static void bindValue(SqlServerDefaultTestBytes& entity, const QString& target, QVariant value) {
            if (target == "typebit") {
                entity.typebit = value.value<bool>();
            } else if (target == "typebitnull") {
                entity.typebitnull = value.value<bool>();
            } else if (target == "typebinary") {
                entity.typebinary = value.value<QByteArray>();
            } else if (target == "typebinary2") {
                entity.typebinary2 = value.value<QByteArray>();
            } else if (target == "typebinary3") {
                entity.typebinary3 = value.value<QByteArray>();
            } else if (target == "typebinary4") {
                entity.typebinary4 = value.value<QByteArray>();
            } else if (target == "typebinary5") {
                entity.typebinary5 = value.value<QByteArray>();
            } else if (target == "typevarbinary") {
                entity.typevarbinary = value.value<QByteArray>();
            } else if (target == "typevarbinary2") {
                entity.typevarbinary2 = value.value<QByteArray>();
            } else if (target == "typevarbinary3") {
                entity.typevarbinary3 = value.value<QByteArray>();
            } else if (target == "typevarbinary4") {
                entity.typevarbinary4 = value.value<QByteArray>();
            } else if (target == "typevarbinary5") {
                entity.typevarbinary5 = value.value<QByteArray>();
            } else if (target == "typevarbinarymax") {
                entity.typevarbinarymax = value.value<QByteArray>();
            } else if (target == "typevarbinarymax2") {
                entity.typevarbinarymax2 = value.value<QByteArray>();
            } else if (target == "typevarbinarymax3") {
                entity.typevarbinarymax3 = value.value<QByteArray>();
            } else if (target == "typevarbinarymax4") {
                entity.typevarbinarymax4 = value.value<QByteArray>();
            } else if (target == "typevarbinarymax5") {
                entity.typevarbinarymax5 = value.value<QByteArray>();
            } else if (target == "typevariant") {
                entity.typevariant = value.value<QVariant>();
            } else if (target == "typeuniqueidentifier") {
                entity.typeuniqueidentifier = value.value<QByteArray>();
            } else if (target == "typexml") {
                entity.typexml = value.value<QByteArray>();
            } else {
                entity.__putExtra(target, value);
            }
        }

        static SqlServerDefaultTestBytes fromJson(const QJsonObject& object) {
            SqlServerDefaultTestBytes entity;
            entity.typebit = object.value("typebit").toVariant().value<bool>();
            entity.typebitnull = object.value("typebitnull").toVariant().value<bool>();
            entity.typebinary = QByteArray::fromBase64(object.value("typebinary").toString().toLatin1());
            entity.typebinary2 = QByteArray::fromBase64(object.value("typebinary2").toString().toLatin1());
            entity.typebinary3 = QByteArray::fromBase64(object.value("typebinary3").toString().toLatin1());
            entity.typebinary4 = QByteArray::fromBase64(object.value("typebinary4").toString().toLatin1());
            entity.typebinary5 = QByteArray::fromBase64(object.value("typebinary5").toString().toLatin1());
            entity.typevarbinary = QByteArray::fromBase64(object.value("typevarbinary").toString().toLatin1());
            entity.typevarbinary2 = QByteArray::fromBase64(object.value("typevarbinary2").toString().toLatin1());
            entity.typevarbinary3 = QByteArray::fromBase64(object.value("typevarbinary3").toString().toLatin1());
            entity.typevarbinary4 = QByteArray::fromBase64(object.value("typevarbinary4").toString().toLatin1());
            entity.typevarbinary5 = QByteArray::fromBase64(object.value("typevarbinary5").toString().toLatin1());
            entity.typevarbinarymax = QByteArray::fromBase64(object.value("typevarbinarymax").toString().toLatin1());
            entity.typevarbinarymax2 = QByteArray::fromBase64(object.value("typevarbinarymax2").toString().toLatin1());
            entity.typevarbinarymax3 = QByteArray::fromBase64(object.value("typevarbinarymax3").toString().toLatin1());
            entity.typevarbinarymax4 = QByteArray::fromBase64(object.value("typevarbinarymax4").toString().toLatin1());
            entity.typevarbinarymax5 = QByteArray::fromBase64(object.value("typevarbinarymax5").toString().toLatin1());
            entity.typevariant = object.value("typevariant");
            entity.typeuniqueidentifier = QByteArray::fromBase64(object.value("typeuniqueidentifier").toString().toLatin1());
            entity.typexml = QByteArray::fromBase64(object.value("typexml").toString().toLatin1());
            return entity;
        }

        static QJsonObject toJson(const SqlServerDefaultTestBytes& entity, QStringList excludeKeys = QStringList()) {
            QJsonObject object;
            object.insert("typebit", entity.typebit);
            object.insert("typebitnull", entity.typebitnull);
            object.insert("typebinary", QString::fromLatin1(entity.typebinary.toBase64()));
            object.insert("typebinary2", QString::fromLatin1(entity.typebinary2.toBase64()));
            object.insert("typebinary3", QString::fromLatin1(entity.typebinary3.toBase64()));
            object.insert("typebinary4", QString::fromLatin1(entity.typebinary4.toBase64()));
            object.insert("typebinary5", QString::fromLatin1(entity.typebinary5.toBase64()));
            object.insert("typevarbinary", QString::fromLatin1(entity.typevarbinary.toBase64()));
            object.insert("typevarbinary2", QString::fromLatin1(entity.typevarbinary2.toBase64()));
            object.insert("typevarbinary3", QString::fromLatin1(entity.typevarbinary3.toBase64()));
            object.insert("typevarbinary4", QString::fromLatin1(entity.typevarbinary4.toBase64()));
            object.insert("typevarbinary5", QString::fromLatin1(entity.typevarbinary5.toBase64()));
            object.insert("typevarbinarymax", QString::fromLatin1(entity.typevarbinarymax.toBase64()));
            object.insert("typevarbinarymax2", QString::fromLatin1(entity.typevarbinarymax2.toBase64()));
            object.insert("typevarbinarymax3", QString::fromLatin1(entity.typevarbinarymax3.toBase64()));
            object.insert("typevarbinarymax4", QString::fromLatin1(entity.typevarbinarymax4.toBase64()));
            object.insert("typevarbinarymax5", QString::fromLatin1(entity.typevarbinarymax5.toBase64()));
            object.insert("typevariant", QJsonValue::fromVariant(entity.typevariant));
            object.insert("typeuniqueidentifier", QString::fromLatin1(entity.typeuniqueidentifier.toBase64()));
            object.insert("typexml", QString::fromLatin1(entity.typexml.toBase64()));

            for (const auto& key : excludeKeys) {
                object.remove(key);
            }
            return object;
        }
    };

public:
    //
    inline void setTypebit(const bool& typebit) {this->typebit = typebit;}
    //
    inline bool getTypebit() const {return typebit;}
    //
    inline void setTypebitnull(const bool& typebitnull) {this->typebitnull = typebitnull;}
    //
    inline bool getTypebitnull() const {return typebitnull;}
    //
    inline void setTypebinary(const QByteArray& typebinary) {this->typebinary = typebinary;}
    //
    inline QByteArray getTypebinary() const {return typebinary;}
    //
    inline void setTypebinary2(const QByteArray& typebinary2) {this->typebinary2 = typebinary2;}
    //
    inline QByteArray getTypebinary2() const {return typebinary2;}
    //
    inline void setTypebinary3(const QByteArray& typebinary3) {this->typebinary3 = typebinary3;}
    //
    inline QByteArray getTypebinary3() const {return typebinary3;}
    //
    inline void setTypebinary4(const QByteArray& typebinary4) {this->typebinary4 = typebinary4;}
    //
    inline QByteArray getTypebinary4() const {return typebinary4;}
    //
    inline void setTypebinary5(const QByteArray& typebinary5) {this->typebinary5 = typebinary5;}
    //
    inline QByteArray getTypebinary5() const {return typebinary5;}
    //
    inline void setTypevarbinary(const QByteArray& typevarbinary) {this->typevarbinary = typevarbinary;}
    //
    inline QByteArray getTypevarbinary() const {return typevarbinary;}
    //
    inline void setTypevarbinary2(const QByteArray& typevarbinary2) {this->typevarbinary2 = typevarbinary2;}
    //
    inline QByteArray getTypevarbinary2() const {return typevarbinary2;}
    //
    inline void setTypevarbinary3(const QByteArray& typevarbinary3) {this->typevarbinary3 = typevarbinary3;}
    //
    inline QByteArray getTypevarbinary3() const {return typevarbinary3;}
    //
    inline void setTypevarbinary4(const QByteArray& typevarbinary4) {this->typevarbinary4 = typevarbinary4;}
    //
    inline QByteArray getTypevarbinary4() const {return typevarbinary4;}
    //
    inline void setTypevarbinary5(const QByteArray& typevarbinary5) {this->typevarbinary5 = typevarbinary5;}
    //
    inline QByteArray getTypevarbinary5() const {return typevarbinary5;}
    //
    inline void setTypevarbinarymax(const QByteArray& typevarbinarymax) {this->typevarbinarymax = typevarbinarymax;}
    //
    inline QByteArray getTypevarbinarymax() const {return typevarbinarymax;}
    //
    inline void setTypevarbinarymax2(const QByteArray& typevarbinarymax2) {this->typevarbinarymax2 = typevarbinarymax2;}
    //
    inline QByteArray getTypevarbinarymax2() const {return typevarbinarymax2;}
    //
    inline void setTypevarbinarymax3(const QByteArray& typevarbinarymax3) {this->typevarbinarymax3 = typevarbinarymax3;}
    //
    inline QByteArray getTypevarbinarymax3() const {return typevarbinarymax3;}
    //
    inline void setTypevarbinarymax4(const QByteArray& typevarbinarymax4) {this->typevarbinarymax4 = typevarbinarymax4;}
    //
    inline QByteArray getTypevarbinarymax4() const {return typevarbinarymax4;}
    //
    inline void setTypevarbinarymax5(const QByteArray& typevarbinarymax5) {this->typevarbinarymax5 = typevarbinarymax5;}
    //
    inline QByteArray getTypevarbinarymax5() const {return typevarbinarymax5;}
    //
    inline void setTypevariant(const QVariant& typevariant) {this->typevariant = typevariant;}
    //
    inline QVariant getTypevariant() const {return typevariant;}
    //
    inline void setTypeuniqueidentifier(const QByteArray& typeuniqueidentifier) {this->typeuniqueidentifier = typeuniqueidentifier;}
    //
    inline QByteArray getTypeuniqueidentifier() const {return typeuniqueidentifier;}
    //
    inline void setTypexml(const QByteArray& typexml) {this->typexml = typexml;}
    //
    inline QByteArray getTypexml() const {return typexml;}
    //set temp data
    inline void __putExtra(const QString& key, const QVariant& extra) {this->__extra.insert(key, extra);}
    //get function select result, like get "as" field result
    inline QVariant __getExtra(const QString& key) const {return __extra.value(key);}
};
typedef QList<SqlServerDefaultTestBytes> SqlServerDefaultTestBytesList;
Q_DECLARE_METATYPE(SqlServerDefaultTestBytes);
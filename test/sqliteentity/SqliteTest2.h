/*This file is auto generated by DbEntityGenerator*/
#pragma once

#include <qobject.h>
#include <qvariant.h>

#include "../../src/condition/EntityField.h"

class SqliteTest2 {
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

public:
    SqliteTest2() {
        number = 0;
    }

    SqliteTest2(
        qint64 id,
        const QString& name,
        int number,
        int number2,
        const QVariant& varianttype
    ) : id(id)
    , name(name)
    , number(number)
    , number2(number2)
    , varianttype(varianttype)
    { }

public:
    struct Fields {
        EntityField<qint64> id = EntityField<qint64>("id");
        EntityField<QString> name = EntityField<QString>("name");
        EntityField<int> number = EntityField<int>("number");
        EntityField<int> number2 = EntityField<int>("number2");
        EntityField<QVariant> varianttype = EntityField<QVariant>("varianttype");
    };

    struct Info {
        static int fieldSize() {
            return 5;
        }

        static QString getTableName() {
            return QStringLiteral("ts_sqlitetest2");
        }

        static QStringList getFields() {
            return QStringList()
                << "id"
                << "name"
                << "number"
                << "number2"
                << "varianttype";
        }

        static QStringList getFieldsType() {
            return QStringList() 
                << QStringLiteral("id integer primary key autoincrement")
                << QStringLiteral("name text")
                << QStringLiteral("number integer default 0")
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

public:
    //set 自增长主键
    inline void setId(qint64 id) {this->id = id;}
    //get 自增长主键
    inline qint64 getId() const {return id;}
    //
    inline void setName(const QString& name) {this->name = name;}
    //
    inline QString getName() const {return name;}
    //
    inline void setNumber(int number) {this->number = number;}
    //
    inline int getNumber() const {return number;}
    //
    inline void setNumber2(int number2) {this->number2 = number2;}
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
};
typedef QList<SqliteTest2> SqliteTest2List;

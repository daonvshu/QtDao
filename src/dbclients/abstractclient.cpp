#include "dbclients/abstractclient.h"

#include "query/basequery.h"

#include "dbexceptionhandler.h"

QTDAO_BEGIN_NAMESPACE

void AbstractClient::restoreData2NewTable(const QString& tbname, QStringList fields) {
    auto oldTbFields = getTagTableFields("tmp_" + tbname);
    if (oldTbFields.isEmpty()) {
        return;
    }
    QString fieldsStr;
    for (int i = 0; i < oldTbFields.size(); i++) {
        auto field = oldTbFields.at(i);
        if (fields.contains(field)) {
            fieldsStr.append(field).append(",");
        }
    }
    if (!fieldsStr.isEmpty()) {
        fieldsStr.chop(1);
        auto sql = QString("insert into %1(%2) select %2 from %3")
            .arg(tbname, fieldsStr, "tmp_" + tbname);
        BaseQuery::setErrIfQueryFail(DbErrCode::DATABASE_INIT_FAIL);
        restoreDataBefore(tbname);
        BaseQuery::queryPrimitiveThrowable(sql);
        restoreDataAfter(tbname);
    }
}

void AbstractClient::createTableIfNotExist(const QString& tbName, QStringList fieldsType, QStringList primaryKeys) {
    Q_UNUSED(tbName);
    Q_UNUSED(fieldsType);
    Q_UNUSED(primaryKeys);
}

void AbstractClient::createTableIfNotExist(const QString& tbName, const QString& engine, QStringList fieldsType, QStringList primaryKeys) {
    Q_UNUSED(tbName);
    Q_UNUSED(engine);
    Q_UNUSED(fieldsType);
    Q_UNUSED(primaryKeys);
}

void AbstractClient::createIndex(const QString& tbName, QStringList fields, IndexType type) {
    Q_UNUSED(tbName);
    Q_UNUSED(fields);
    Q_UNUSED(type);
}

void AbstractClient::createIndex(const QString& tbName, QStringList fields, IndexType type, const std::function<QString(const QString&)>& optionGet) {
    Q_UNUSED(tbName);
    Q_UNUSED(fields);
    Q_UNUSED(type);
    Q_UNUSED(optionGet);
}

void AbstractClient::restoreDataBefore(const QString& tbName) { Q_UNUSED(tbName) }

void AbstractClient::restoreDataAfter(const QString& tbName) { Q_UNUSED(tbName) }

QString AbstractClient::translateSqlStatement(const QString& statement, const QVariantList& values) {
    QString tmp = statement;
    int placeholdIndex;
    int vi = 0;
    QList<QVariant::Type> stringType;
    stringType << QVariant::String;
    stringType << QVariant::Date;
    stringType << QVariant::DateTime;
    stringType << QVariant::Char;
    while ((placeholdIndex = tmp.indexOf('?')) != -1) {
        QVariant value = values.at(vi++);
        QString valueStr;
        if (stringType.contains(value.type())) {
            valueStr = " '" + value.toString() + "' ";
        } else if (value.type() == QVariant::ByteArray) {
            valueStr = "0x" + value.toByteArray().toHex();
        } else {
            valueStr = value.toString();
        }
        tmp.replace(placeholdIndex, 1, valueStr);
    }
    return tmp;
}

QTDAO_END_NAMESPACE
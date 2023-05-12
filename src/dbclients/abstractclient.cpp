#include "dbclients/abstractclient.h"

#include "query/basequery.h"
#include "config/configbuilder.h"

#include "dbexception.h"

QTDAO_BEGIN_NAMESPACE

void AbstractClient::restoreData2NewTable(const QString& tbName, const QStringList& fields) {
    auto oldTbFields = getTagTableFields("tmp_" + tbName);
    if (oldTbFields.isEmpty()) {
        return;
    }
    QString fieldsStr;
    for (const auto & field : oldTbFields) {
        if (fields.contains(field)) {
            fieldsStr.append(field).append(",");
        }
    }
    if (!fieldsStr.isEmpty()) {
        fieldsStr.chop(1);
        auto sql = QString("insert into %1(%2) select %2 from %3")
            .arg(tbName, fieldsStr, "tmp_" + tbName);
        restoreDataBefore(tbName);
        BaseQuery::queryPrimitive(sql);
        restoreDataAfter(tbName);
    }
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
        const QVariant& value = values.at(vi++);
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

QString AbstractClient::checkAndRemoveKeywordEscapes(const QString& tbOrFieldName, const QStringList& symbols) {
    auto name = tbOrFieldName;
    for (const auto& s : symbols) {
        name.remove(s);
    }
    return name;
}

QString AbstractClient::currentDatabaseName() {
    return globalConfig->mDatabaseName;
}

void AbstractClient::tableUpgrade(EntityReaderInterface *reader, int oldVersion, int curVersion) {
    auto upgrader = globalConfig->dbUpgrader;
    upgrader->setEntityReader(reader);
    upgrader->onUpgrade(oldVersion, curVersion);
}

QTDAO_END_NAMESPACE
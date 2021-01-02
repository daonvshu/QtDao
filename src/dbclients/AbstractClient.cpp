#include "AbstractClient.h"

#include "../query/BaseQuery.h"

#include "../DbExceptionHandler.h"

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
        restoreDataBefore(tbname);
        BaseQuery::queryPrimitiveThrowable(sql);
        restoreDataAfter(tbname);
    }
}

void AbstractClient::createTableIfNotExist(const QString& tbName, QStringList fieldsType, QStringList primaryKeys) {
}

void AbstractClient::createTableIfNotExist(const QString& tbName, const QString& engine, QStringList fieldsType, QStringList primaryKeys) {
}

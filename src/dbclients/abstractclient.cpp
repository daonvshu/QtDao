#include "dbclients/abstractclient.h"

#include "query/basequery.h"
#include "config/configbuilder.h"

#include "dbexception.h"

#include <qset.h>
#include <qstringbuilder.h>

QTDAO_BEGIN_NAMESPACE

void AbstractClient::createTableIfNotExist(dao::EntityReaderInterface *reader) {
    createTableIfNotExist(reader->getTableName(), reader->getFieldsType(), reader->getPrimaryKeys(),
                          reader->getForeignKeys(), reader->getTableEngine());
}

void AbstractClient::dropAllIndexOnTable(const QString &tbName) {
    auto indexGroup = exportAllIndexes(tbName);
    for (const auto& indexes : indexGroup) {
        for (const auto& i : indexes) {
            dropIndex(tbName, i);
        }
    }
}

void AbstractClient::createIndex(dao::EntityReaderInterface *reader) {
    //sqlite/mysql
    auto normalIndexFields = reader->getIndexFields();
    for (const auto& i : normalIndexFields) {
        createIndex(reader->getTableName(), i, IndexType::INDEX_NORMAL);
    }

    auto uniqueIndexFields = reader->getUniqueIndexFields();
    for (const auto& i : uniqueIndexFields) {
        createIndex(reader->getTableName(), i, IndexType::INDEX_UNIQUE);
    }

    auto optionGetter = [=] (const QString& indexName) {
        return reader->getIndexOption(indexName);
    };

    //sqlserver
    auto clusteredIndexFields = reader->getClusteredIndexFields();
    for (const auto& i : clusteredIndexFields) {
        createIndex(reader->getTableName(), i, IndexType::INDEX_CLUSTERED, optionGetter);
    }
    //create unique clustered index
    auto uniqueClusteredIndexFields = reader->getUniqueClusteredIndexFields();
    for (const auto& i : uniqueClusteredIndexFields) {
        createIndex(reader->getTableName(), i, IndexType::INDEX_UNIQUE_CLUSTERED, optionGetter);
    }
    //create non-clustered index
    auto nonclusteredIndexFields = reader->getNonClusteredIndexFields();
    for (const auto& i : nonclusteredIndexFields) {
        createIndex(reader->getTableName(), i, IndexType::INDEX_NONCLUSTERED, optionGetter);
    }
    //create unique non-clustered index
    auto uniqueNonclusteredIndexFields = reader->getUniqueNonClusteredIndexFields();
    for (const auto& i : uniqueNonclusteredIndexFields) {
        createIndex(reader->getTableName(), i, IndexType::INDEX_UNIQUE_NONCLUSTERED, optionGetter);
    }
}

void AbstractClient::createIndex(const QString &tbName, const QStringList &fields, IndexType type,
                                 const std::function<QString(const QString &)> &indexOption)
 {
    auto indexName = getIndexFromFields(fields);
    createIndex(tbName, indexName, fields, type, indexOption == nullptr ? "" : indexOption(indexName));
}

void AbstractClient::dropIndex(const QString &tbName, const QStringList &fields) {
    dropIndex(tbName, getIndexFromFields(fields));
}

QStringList AbstractClient::getIndexArrayFromFields(const QList<QStringList> &fieldArray) {
    QStringList names;
    for (const auto& fields : fieldArray) {
        names << getIndexFromFields(fields);
    }
    return names;
}

void AbstractClient::transferData(const QString& fromTb, const QString& toTb) {
    auto oldTbFields = exportAllFields(fromTb);
    Q_ASSERT(!oldTbFields.isEmpty());

    auto newTbFields = exportAllFields(toTb);
    QSet<QString> newTbFieldSet;
    for (const auto& field : newTbFields) {
        newTbFieldSet.insert(field.first);
    }

    QString fieldsStr;
    for (const auto & field : oldTbFields) {
        if (newTbFieldSet.contains(field.first)) {
            if (field.second.toUpper() != "TIMESTAMP") { //ignore timestamp of sqlserver
                fieldsStr.append(field.first).append(",");
            }
        }
    }
    if (!fieldsStr.isEmpty()) {
        fieldsStr.chop(1);
        BaseQuery::queryPrimitive("insert into " % toTb % "(" % fieldsStr % ") select " % fieldsStr % " from " % fromTb);
    }
}

QString AbstractClient::translateSqlStatement(const QString& statement, const QVariantList& values) {
    QString tmp = statement;
    int vi = 0;

#if QT_VERSION_MAJOR >= 6
    qsizetype placeHoldIndex;

    QList<QMetaType::Type> stringType;
    stringType << QMetaType::QString;
    stringType << QMetaType::QDate;
    stringType << QMetaType::QDateTime;
    stringType << QMetaType::Char;
    while ((placeHoldIndex = tmp.indexOf('?')) != -1) {
        const QVariant& value = values.at(vi++);
        QString valueStr;
        if (stringType.contains(value.typeId())) {
            valueStr = " '" + value.toString() + "' ";
        } else if (value.typeId() == QMetaType::QByteArray) {
            valueStr = "0x" + value.toByteArray().toHex();
        } else {
            valueStr = value.toString();
        }
        tmp.replace(placeHoldIndex, 1, valueStr);
    }
#else
    int placeHoldIndex;

    QList<QVariant::Type> stringType;
    stringType << QVariant::String;
    stringType << QVariant::Date;
    stringType << QVariant::DateTime;
    stringType << QVariant::Char;
    while ((placeHoldIndex = tmp.indexOf('?')) != -1) {
        const QVariant& value = values.at(vi++);
        QString valueStr;
        if (stringType.contains(value.type())) {
            valueStr = " '" + value.toString() + "' ";
        } else if (value.type() == QVariant::ByteArray) {
            valueStr = "0x" + value.toByteArray().toHex();
        } else {
            valueStr = value.toString();
        }
        tmp.replace(placeHoldIndex, 1, valueStr);
    }
#endif
    return tmp;
}

QString AbstractClient::checkAndRemoveKeywordEscapes(const QString& tbOrFieldName, const QStringList& symbols) {
    auto name = tbOrFieldName;
    for (const auto& s : symbols) {
        name.remove(s);
    }
    return name;
}

QString AbstractClient::getIndexFromFields(const QStringList &fields, const QStringList &escapeSymbols) {
    QString indexName = "index";
    for (const auto& field : fields) {
        indexName.append("_").append(checkAndRemoveKeywordEscapes(field.split(" ").at(0), escapeSymbols));
    }
    return indexName;
}

QString AbstractClient::currentDatabaseName() const {
    return config->mDatabaseName;
}

qint64 AbstractClient::currentSessionId() const {
    return config->mSessionId;
}

void AbstractClient::tableUpgrade(EntityReaderInterface *reader, int oldVersion, int curVersion) {
    auto upgrader = config->dbUpgrader;
    upgrader->setEntityReader(reader);
    upgrader->onUpgrade(oldVersion, curVersion);
}

QString AbstractClient::translateForeignKeyStatement(const ForeignKey &key) {
    QString str = QLatin1String("foreign key(%1) references %2(%3)");
    str = str.arg(key.fieldKeys.join(','), key.referenceTb, key.referenceKeys.join(','));
    QList<ForeignKey::Action> actions = { key.onUpdate, key.onDelete };
    for (int i = 0; i < actions.size(); i++) {
        const auto& action = actions[i];
        if (action != ForeignKey::FK_NotSet) {
            if (i == 0) {
                str += " on update ";
            } else {
                str += " on delete ";
            }
            switch (action) {
                case ForeignKey::FK_NO_ACTION:
                    str += "no action";
                    break;
                case ForeignKey::FK_RESTRICT:
                    str += "restrict";
                    break;
                case ForeignKey::FK_SET_NULL:
                    str += "set null";
                    break;
                case ForeignKey::FK_SET_DEFAULT:
                    str += "set default";
                    break;
                case ForeignKey::FK_CASCADE:
                    str += "cascade";
                    break;
                case ForeignKey::FK_NotSet:
                    break;
            }
        }
    }
    if (key.deferrable) {
        str += " DEFERRABLE INITIALLY DEFERRED"; //deferred foreign key constraint
    }
    return str;
}

QTDAO_END_NAMESPACE
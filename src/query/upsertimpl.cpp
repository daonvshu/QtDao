#include "query/upsertimpl.h"

#include "config/configbuilder.h"
#include "config/configmanager.h"

QTDAO_BEGIN_NAMESPACE

bool UpsertImpl::buildInsertBySetSqlStatement() {

    auto& sc = setConnector();
    sc.combine();

    auto usedFieldName = sc.getUsedFields();
    QVariantList values = sc.getValueList();
    Q_ASSERT(!values.isEmpty());

#if QT_VERSION_MAJOR >= 6
    bool operateBatch = values.at(0).typeId() == QMetaType::QVariantList;
#else
    bool operateBatch = values.at(0).type() == QVariant::List;
#endif

    QStringList usedFields;
    for (int i = 0; i < usedFieldName.size(); ) {
        auto field = usedFieldName.at(i);
        if (!isAutoIncrement(field.name)) {
            usedFields << field.name;
            i++;
        } else {
            usedFieldName.removeAt(i);
            values.removeAt(i);
        }
    }

    auto sql = buildInsertStatement(usedFields, [&] (const QString& fieldName) -> QVariant {
        return values.at(usedFields.indexOf(fieldName));
    });
    values.append(this->values);
    setSqlQueryStatement(sql, values);

    return operateBatch;
}

QString UpsertImpl::buildInsertStatement(const QStringList &fields, const std::function<QVariant(const QString&)>& fieldToValue) {

    const auto usedFieldListStr = [&] {
        QString str;
        for (const auto& field: fields) {
            str += field + ",";
        }
        return str.chopped(1);
    };

    const auto valuePlaceholder = [&] {
        return QString("?,").repeated(fields.size()).chopped(1);
    };

    const auto usedUpdateFieldNames = [&] {
        auto& updateCols = updateColumnConnector();
        updateCols.combine();
        auto usedFieldNames = listMap<QString, FieldInfo>(updateCols.getUsedFields(), [&](const FieldInfo& info) {
            return info.name;
        });
        if (usedFieldNames.isEmpty()) {
            usedFieldNames = fields;
        }
        return usedFieldNames;
    };

    auto config = ConfigManager::getConfig(getSessionId());
    if (config->isSqlite() || config->isPSql()) {
        QString statementTemplate = "insert into %1 (%2) values(%3) on conflict(%4) do update set %5";

        //conflict field
        auto& conflictCols = conflictColumnConnector();
        conflictCols.combine();
        QString conflictFields;
        QSet<QString> conflictFieldNames;
        for (const auto& field: conflictCols.getUsedFields()) {
            conflictFields += field.name + ",";
            conflictFieldNames << field.name;
        }
        conflictFields.chop(1);

        //update fields
        QString updateStr;
        for (const auto& fieldName: usedUpdateFieldNames()) {
            if (conflictFieldNames.contains(fieldName)) continue;
            updateStr += fieldName + "=excluded." + fieldName + ",";
        }
        updateStr.chop(1);

        auto& fc = filterConnector();
        if (!fc.isEmpty()) {
            fc.setFieldPrefixGetter([&](const QString& tableName) {
                return tableName + '.';
            });
            fc.combine();
            updateStr.append(" where ").append(fc.getConditionSegment());
            values << fc.getValueList();
        }

        return statementTemplate.arg(getTableName(), usedFieldListStr(), valuePlaceholder(),
                                     conflictFields, updateStr);

    }

    if (config->isMysql()) {
        QString statementTemplate = "insert into %1 (%2) values(%3) on duplicate key update %4";

        auto& conflictCols = conflictColumnConnector();
        conflictCols.combine();
        QSet<QString> conflictFieldNames;
        for (const auto& field: conflictCols.getUsedFields()) {
            conflictFieldNames << field.name;
        }

        //update fields
        QString updateStr;
        for (const auto& fieldName: usedUpdateFieldNames()) {
            if (conflictFieldNames.contains(fieldName)) continue;
            updateStr += fieldName + "=?,";
            values.append(fieldToValue(fieldName));
        }
        updateStr.chop(1);

        return statementTemplate.arg(getTableName(), usedFieldListStr(), valuePlaceholder(),
                                     updateStr);

    }

    if (config->isPSql()) {

    }

    if (config->isSqlServer()) {
        QString statementTemplate = "merge into %1 a\n"
                                    "using (select %2) b\n"
                                    "on %3\n"
                                    "when matched then\n"
                                    "update set %4\n"
                                    "when not matched then\n"
                                    "insert(%5) values(%6);";

        //temp value table
        QString tempValueFields;
        for (const auto& field : fields) {
            tempValueFields += "? as " + field + ", ";
        }
        tempValueFields.chop(2);

        auto& conflictCols = conflictColumnConnector();
        //on condition
        conflictCols.combine();
        QString onConditionStr;
        QSet<QString> conflictFieldNames;
        for (const auto& fieldInfo: conflictCols.getUsedFields()) {
            onConditionStr += "a." + fieldInfo.name + "=b." + fieldInfo.name + " and ";
            conflictFieldNames << fieldInfo.name;
        }
        onConditionStr.chop(5);

        //update string
        QString updateStr;
        for (const auto& fieldName: usedUpdateFieldNames()) {
            if (conflictFieldNames.contains(fieldName)) continue;
            updateStr += "a." + fieldName + "=b." + fieldName + ",";
        }
        updateStr.chop(1);

        //insert string
        QString insertStrFields, insertStrValues;
        for (const auto& field: fields) {
            insertStrFields += field + ",";
            insertStrValues += "b." + field + ",";
        }
        insertStrFields.chop(1);
        insertStrValues.chop(1);

        return statementTemplate.arg(getTableName(), tempValueFields, onConditionStr,
                                     updateStr, insertStrFields, insertStrValues);
    }
    return {};
}

QTDAO_END_NAMESPACE
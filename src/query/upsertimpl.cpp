#include "query/upsertimpl.h"

#include "config/configbuilder.h"

QTDAO_BEGIN_NAMESPACE

bool UpsertImpl::buildInsertBySetSqlStatement() {

    builder->setCondition.connect();

    auto usedFieldName = builder->setCondition.getUsedFieldNames();
    QVariantList values = builder->setCondition.getValues();
    Q_ASSERT(!values.isEmpty());

    bool operateBatch = values.at(0).type() == QVariant::List;

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

    if (globalConfig->isSqlite()) {
        QString statementTemplate = "insert into %1 (%2) values(%3) on conflict(%4) do update set %5";
        //conflict field
        builder->conflictCols.connect();
        QString conflictFields;
        for (const auto& field: builder->conflictCols.getUsedFieldNames()) {
            conflictFields += field.name + ",";
        }
        conflictFields.chop(1);
        //update fields
        builder->updateCols.connect();
        QString updateStr;
        for (const auto& fieldInfo: builder->updateCols.getUsedFieldNames()) {
            updateStr += fieldInfo.name + "=excluded." + fieldInfo.name + ",";
        }
        updateStr.chop(1);
        return statementTemplate.arg(getTableName(), usedFieldListStr(), valuePlaceholder(),
                                     conflictFields, updateStr);

    } else if (globalConfig->isMysql()) {
        QString statementTemplate = "insert into %1 (%2) values(%3) on duplicate key update %4";
        //update fields
        builder->updateCols.connect();
        QString updateStr;
        for (const auto& fieldInfo: builder->updateCols.getUsedFieldNames()) {
            updateStr += fieldInfo.name + "=?,";
            values.append(fieldToValue(fieldInfo.name));
        }
        updateStr.chop(1);
        return statementTemplate.arg(getTableName(), usedFieldListStr(), valuePlaceholder(),
                                     updateStr);

    } else if (globalConfig->isSqlServer()) {
        QString statementTemplate = "merge info %1 a\n"
                                    "using (select %2) b\n"
                                    "on %3\n"
                                    "when matched then\n"
                                    "update set %4\n"
                                    "when not matched then\n"
                                    "insert(%5) values(%6);";
        //temp value table
        QString tempValueFields;
        for (const auto& field : fields) {
            tempValueFields += "? as " + field + ",";
        }
        tempValueFields.chop(1);
        //on condition
        builder->conflictCols.connect();
        QString onConditionStr;
        for (const auto& fieldInfo: builder->conflictCols.getUsedFieldNames()) {
            onConditionStr += "a." + fieldInfo.name + "=b." + fieldInfo.name + ",";
        }
        onConditionStr.chop(1);
        //update string
        builder->updateCols.connect();
        QString updateStr;
        for (const auto& fieldInfo: builder->updateCols.getUsedFieldNames()) {
            updateStr += "a." + fieldInfo.name + "=b." + fieldInfo.name + ",";
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
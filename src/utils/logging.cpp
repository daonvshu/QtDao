#include "utils/logging.h"
#include "utils/listutils.h"
#include "query/basequery.h"

QTDAO_BEGIN_NAMESPACE

const QString logStatementTemplate = "\n>>>--------------------------------------------------------<<<\n"
                                     "category:        %0\n"
                                     "build the query: %1\n"
                                     "--------------------------------------------------------------\n";

const QString logStatementValuesTemplate = "\n>>>--------------------------------------------------------<<<\n"
                                           "category:        %0\n"
                                           "build the query: %1\n"
                                           "values:          %2\n"
                                           "translated:      %3\n"
                                           "--------------------------------------------------------------\n";

QString variantTypeFormat(const QVariant& v) {
    switch (v.type()) {
        //case QVariant::Double:
        //    break;
        case QVariant::Char:
            return '\'' + v.toString() + '\'';
        case QVariant::ByteArray:
            return "<" + QString::number(v.toByteArray().size()) + "bits>";
        case QVariant::String:
        case QVariant::Date:
        case QVariant::Time:
        case QVariant::DateTime:
            return '"' + v.toString() + '"';
        default:
            return v.toString();
    }
}

QString variantTypeFormatSql(const QVariant& v) {
    switch (v.type()) {
        //case QVariant::Double:
        //    break;
        case QVariant::ByteArray:
            return "<" + QString::number(v.toByteArray().size()) + "bits>";
        case QVariant::String:
        case QVariant::Date:
        case QVariant::Time:
        case QVariant::DateTime:
        case QVariant::Char:
            return '\'' + v.toString() + '\'';
        default:
            return v.toString();
    }
}

void printStatement(LoggingCategoryPtr loggingPtr, const QString& statement) {
    qCDebug(loggingPtr).noquote() << logStatementTemplate.arg(loggingPtr().categoryName(), statement);
}

void printStatementValues(LoggingCategoryPtr loggingPtr, const QString& statement, const QVariantList& values) {
    QString valueStr = "[";
    for (const auto& v : values) {
        valueStr += variantTypeFormat(v);
        valueStr += " ";
    }
    valueStr.chop(1);
    valueStr += "]";

    QString translatedStr = statement;
    int index;
    int offset = 0;
    int valueIndex = 0;
    while ((index = translatedStr.indexOf("?", offset)) != -1) {
        auto valueFormatted = variantTypeFormatSql(values.at(valueIndex++));
        translatedStr.remove(index, 1).insert(index, valueFormatted);
        if (valueIndex >= values.size()) {
            break;
        }
        offset = index + valueFormatted.size();
    }

    qCDebug(loggingPtr).noquote() << logStatementValuesTemplate.arg(loggingPtr().categoryName(), statement, valueStr, translatedStr);
}

void printStatementValuesBatch(LoggingCategoryPtr loggingPtr, const QString& statement, const QVariantList& values) {

    auto arrayValues = listMap<QVariantList, QVariant>(values, [](const QVariant& v) {
        return v.value<QVariantList>();
    });

    QString valueStr = "[";
    for (const auto& v : arrayValues) {
        valueStr += " [";
        int rowIndex = 0;
        for (const auto& i : v) {
            if (++rowIndex > 5) {
                valueStr += "... ";
                break;
            }
            valueStr += variantTypeFormat(i);
            valueStr += " ";
        }
        valueStr.chop(1);
        valueStr += "] ";
    }
    valueStr += "]";

    QStringList translatedStrList;

    int index;
    int offset;
    int valueRowIndex = 0, valueColIndex;
    while (true) {
        QString translatedStr = statement;
        valueColIndex = 0;
        offset = 0;

        bool allRowsOverflow = true;
        while ((index = translatedStr.indexOf("?", offset)) != -1) {
            if (arrayValues.size() > valueColIndex) {
                if (arrayValues.at(valueColIndex).size() > valueRowIndex) {
                    auto valueFormatted = variantTypeFormatSql(arrayValues[valueColIndex++][valueRowIndex]);
                    translatedStr.remove(index, 1).insert(index, valueFormatted);
                    offset = index + valueFormatted.size();
                    allRowsOverflow = false;
                } else {
                    offset = index + 1;
                }
                continue;
            }
            break;
        }
        if (allRowsOverflow) {
            break;
        }
        translatedStrList << translatedStr;
        valueRowIndex++;
    }
    QString translatedStr;
    for (int i = 0; i < qMin(5, translatedStrList.size()); i++) {
        translatedStr += QString("[%1] %2\n                 ").arg(i).arg(translatedStrList.at(i));
    }
    if (translatedStrList.size() > 5) {
        translatedStr += "...";
    } else {
        translatedStr.chop(QByteArray("                 ").length());
    }

    qCDebug(loggingPtr).noquote() << logStatementValuesTemplate.arg(loggingPtr().categoryName(), statement, valueStr, translatedStr);
}

void printQueryLog(BaseQuery* query, bool batchExecMode) {
    if (query->values.isEmpty()) {
        printStatement(query->loggingCategoryPtr, query->statement);
    } else {
        if (!batchExecMode) {
            printStatementValues(query->loggingCategoryPtr, query->statement, query->values);
        } else {
            printStatementValuesBatch(query->loggingCategoryPtr, query->statement, query->values);
        }
    }
}

QTDAO_END_NAMESPACE
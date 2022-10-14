#include "BaseTest.h"

#include "DbExceptionHandler.h"

#include <QtTest>
#include <iostream>
#include <qdebug.h>
#include <Windows.h>

QList<QPair<QString, QVariantList>> BaseTest::cachedSqlLog;

void SqlLogPrinter(const QString& sql, const QVariantList& values) {
    BaseTest::cachedSqlLog << qMakePair(sql, values);
}

BaseTest::BaseTest(EngineModel model) : EngineModelSelector(model) {
    cachedSqlLog.clear();
    daoSetQueryLogPrinter(SqlLogPrinter);
}

void BaseTest::clearCacheAndPrintIfTestFail() {
    if (QTest::currentTestFailed()) {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
        for (const auto& log : cachedSqlLog) {
            std::cout << "----TEST FAIL SQL: " << log.first.toStdString() << std::endl;
            QString values;
            QDebug(&values) << log.second;
            std::cout << "----TEST FAIL VALUES: " << values.toStdString() << std::endl;
        }
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    }
    cachedSqlLog.clear();
}

void BaseTest::clearCache() {
    cachedSqlLog.clear();
}

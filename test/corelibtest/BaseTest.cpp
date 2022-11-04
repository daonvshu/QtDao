#include "BaseTest.h"

#include <QtTest>
#include <iostream>
#include <qdebug.h>

#ifdef Q_CC_MSVC
#include <Windows.h>
#endif

QList<QPair<QString, QVariantList>> BaseTest::cachedSqlLog;

void SqlLogPrinter(const QString& sql, const QVariantList& values) {
    BaseTest::cachedSqlLog << qMakePair(sql, values);
}

BaseTest::BaseTest(EngineModel model) : EngineModelSelector(model) {
    cachedSqlLog.clear();
}

void BaseTest::clearCacheAndPrintIfTestFail() {
    if (QTest::currentTestFailed()) {
#ifdef Q_CC_MSVC
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
#endif
        for (const auto& log : cachedSqlLog) {
            std::cout << "----TEST FAIL SQL: " << log.first.toStdString() << std::endl;
            QString values;
            QDebug(&values) << log.second;
            std::cout << "----TEST FAIL VALUES: " << values.toStdString() << std::endl;
        }
#ifdef Q_CC_MSVC
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
#endif
    }
    cachedSqlLog.clear();
}

void BaseTest::clearCache() {
    cachedSqlLog.clear();
}

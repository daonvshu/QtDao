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

class UnitQueryTestExceptionHandler : public DbExceptionHandler {
public:
    using DbExceptionHandler::DbExceptionHandler;

    void execFail(DbErrCode errcode, const QString& lastErr) override {
        Q_UNUSED(errcode)
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
        std::cout << "----TEST EXECUTE ERR: " << lastErr.toLocal8Bit().toStdString() << std::endl;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    };

    void execWarning(const QString& info) override {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);
        std::cout << "----TEST EXECUTE WARNING: " << info.toLocal8Bit().toStdString() << std::endl;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    };
};

BaseTest::BaseTest(EngineModel model) : EngineModelSelector(model) {
    cachedSqlLog.clear();
    daoSetQueryLogPrinter(SqlLogPrinter);
    DbExceptionHandler::setExceptionHandler(new UnitQueryTestExceptionHandler(this));
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

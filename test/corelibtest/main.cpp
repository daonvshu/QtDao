#include <QtCore/QCoreApplication>

#include <QtTest/QTest>

#include "ConnectionPoolTest.h"
#include "query/BaseQueryTest.h"
#include "query/InsertTest.h"
#include "query/SelectTest.h"
#include "query/UpdateTest.h"
#include "query/DeleteTest.h"
#include "query/JoinTest.h"
#include "query/LoggingTest.h"
#include "query/UpsertTest.h"
#include "loader/DbLoaderTest.h"
#include "condition/ConnectorTest.h"
#include "query/InsertIntoSelectTest.h"

#ifndef QT_DAO_TESTCASE
#include <qfile.h>
#include <iostream>

#ifdef Q_CC_MSVC
#include <Windows.h>
#endif

static bool currentIsDebugging() {
#ifdef Q_CC_MSVC
#ifdef _M_X64
    void *PEB = (void*)(__readgsqword(0x60));
#elif _M_IX86
    void *PEB = (void*)(__readfsdword(0x30));
#endif
    return (bool)*(unsigned char*)((unsigned char*)PEB + 0x002);
#else
    return false;
#endif
}

void setColor() {
    QFile file(QDir::currentPath() + "/test.txt");
    if (file.open(QIODevice::ReadOnly)) {
        while (true) {
            auto line = file.readLine();
            if (line.isEmpty()) break;
#ifdef Q_CC_MSVC
            if (line.startsWith("PASS")) {
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_GREEN);
            } else if (line.startsWith("FAIL")) {
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
            } else if (line.startsWith("Totals")) {
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);
            } else if (line.startsWith("*********")) {
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
            } else if (line.startsWith("QWARN")) {
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);
            } else if (line.startsWith("QDEBUG")) {
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_BLUE);
            }
#endif
            std::cout << line.toStdString();
        }
        std::cout << std::endl;
        file.close();
    }
}
#endif

template<typename... Arg> struct TestRunner;
template<typename T, typename... Arg>
struct TestRunner<T, Arg...> : TestRunner<Arg...> {
#ifdef QT_DAO_TESTCASE
    static int run(EngineModel model, int argc, char *argv[]) {
        T t(model);
        int result = QTest::qExec(&t, argc, argv);
        result += TestRunner<Arg...>::run(model, argc, argv);
        return result;
    }
#else
    static int run(EngineModel model) {
        T t(model);
        int result;
        try {
            if (currentIsDebugging()) {
                result = QTest::qExec(&t, QStringList() << "qtdaocoretest.exe");
            } else {
                result = QTest::qExec(&t, QStringList() << "qtdaocoretest.exe" << "-o" << "test.txt");
                setColor();
            }
        } catch (DaoException& e) {
            Q_UNUSED(e)
            auto validDrivers = QSqlDatabase::drivers();
            Q_UNUSED(validDrivers)
            qFatal("run testcase fail!");
        }

        result += TestRunner<Arg...>::run(model);
        return result;
    }
#endif
};
template<> struct TestRunner<> {
#ifdef QT_DAO_TESTCASE
    static int run(EngineModel model, int argc, char *argv[]) {
        Q_UNUSED(model);
        Q_UNUSED(argc);
        Q_UNUSED(argv);
        return 0;
    }
#else
    static int run(EngineModel model) { Q_UNUSED(model); return 0; }
#endif
};

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

#ifdef QT_DAO_TESTCASE
    QTEST_SET_MAIN_SOURCE_PATH;
#endif
    int result = 0;
    for (int i = 0; i < 3; i++) {
#ifndef QT_DAO_TESTCASE
#ifdef Q_CC_MSVC
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);
#endif
        if (i == 0) {
            std::cout << "------------------------------ Test Sqlite ------------------------------" << std::endl;
        } else if (i == 1) {
            std::cout << "------------------------------ Test Mysql ------------------------------" << std::endl;
        } else if (i == 2) {
            std::cout << "------------------------------ Test SqlServer ------------------------------" << std::endl;
        }
#endif
        result += TestRunner<
            DbLoaderTest,
            ConnectionPoolTest,
            BaseQueryTest,
            ConnectorTest,
            InsertTest,
            SelectTest,
            UpdateTest,
            DeleteTest,
            JoinTest,
            InsertIntoSelectTest,
            LoggingTest,
            UpsertTest
        >
#ifdef QT_DAO_TESTCASE
            ::run(EngineModel(i), argc, argv);
    }
    return result;
#else
            ::run(EngineModel(i));
    }
    if (result != 0) {
#ifdef Q_CC_MSVC
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
#endif
        std::cout << "Not all tests are successful!" << std::endl;
    } else {
#ifdef Q_CC_MSVC
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_GREEN);
#endif
        std::cout << "All tests passed!" << std::endl;
    }
    return a.exec();
#endif
}

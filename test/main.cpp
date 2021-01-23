#include <QtCore/QCoreApplication>

#include <QtTest/QTest>

#include "ConnectionPoolTest.h"
#include "query/BaseQueryTest.h"
#include "query/InsertTest.h"
#include "query/SelectTest.h"
#include "query/UpdateTest.h"
#include "query/DeleteTest.h"
#include "query/JoinTest.h"
#include "loader/DbLoaderTest.h"
#include "condition/ConnectorTest.h"
#include "query/InsertIntoSelectTest.h"
#include "query/SqliteWriteSyncTest.h"

#ifndef QT_DAO_TESTCASE
#include <qfile.h>
#include <iostream>
#include <Windows.h>

void setColor() {
    QFile file(QDir::currentPath() + "/test.txt");
    if (file.open(QIODevice::ReadOnly)) {
        while (true) {
            auto line = file.readLine();
            if (line.isEmpty()) break;
            if (line.startsWith("PASS")) {
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_GREEN);
            } else if (line.startsWith("FAIL")) {
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
            } else if (line.startsWith("Totals")) {
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);
            } else if (line.startsWith("*********")) {
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
            }
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
        int result = QTest::qExec(&t, QStringList() << "QtDao.exe" << "-o" << "test.txt");
        setColor();
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
    for (int i = 0; i < 2; i++) {
#ifndef QT_DAO_TESTCASE
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);
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
            SqliteWriteSyncTest
        >
#ifdef QT_DAO_TESTCASE
            ::run(EngineModel(i), argc, argv);
    }
    return result;
#else
            ::run(EngineModel(i));
    }
    if (result != 0) {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
        std::cout << "Not all tests are successful!" << std::endl;
    } else {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_GREEN);
        std::cout << "All tests passed!" << std::endl;
    }
    return a.exec();
#endif
}

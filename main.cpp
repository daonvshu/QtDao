#include <QtCore/QCoreApplication>

#include <QtTest/qtest.h>

#include "test/ConnectionPoolTest.h"
#include "test/query/BaseQueryTest.h"
#include "test/query/InsertTest.h"
#include "test/query/SelectTest.h"
#include "test/query/UpdateTest.h"
#include "test/query/DeleteTest.h"
#include "test/query/JoinTest.h"
#include "test/loader/DbLoaderTest.h"
#include "test/condition/ConnectorTest.h"

#include <qfile.h>
#include <iostream>
#include <Windows.h>

void setColor() {
    QFile file("test.txt");
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
        file.close();
    }
}

template<typename... Arg> struct TestRunner;
template<typename T, typename... Arg>
struct TestRunner<T, Arg...> : TestRunner<Arg...> {
    static void run(int argc, char* argv[]) {
        T t;
        QTest::qExec(&t, argc, argv);
        setColor();
        __super::run(argc, argv);
    }
};
template<> struct TestRunner<> { static void run(int argc, char* argv[]) {} };

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    TestRunner<
        ConnectionPoolTest,
        BaseQueryTest,
        DbLoaderTest,
        ConnectorTest,
        InsertTest,
        SelectTest,
        UpdateTest,
        DeleteTest,
        JoinTest
    >::run(argc, argv);

    return a.exec();
}

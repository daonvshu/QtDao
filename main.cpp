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
#include "test/query/InsertIntoSelectTest.h"

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
        std::cout << std::endl;
        file.close();
    }
}

template<typename... Arg> struct TestRunner;
template<typename T, typename... Arg>
struct TestRunner<T, Arg...> : TestRunner<Arg...> {
    static int run() {
        T t;
        int result = QTest::qExec(&t, QStringList() << "QtDao.exe" << "-o" << "test.txt");
        setColor();
        result += __super::run();
        return result;
    }
};
template<> struct TestRunner<> { static int run() { return 0; } };

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    int result = TestRunner<
        ConnectionPoolTest,
        BaseQueryTest,
        DbLoaderTest,
        ConnectorTest,
        InsertTest,
        SelectTest,
        UpdateTest,
        DeleteTest,
        JoinTest,
        InsertIntoSelectTest
    >::run();
    if (result != 0) {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
        std::cout << "Not all tests are successful!" << std::endl;
    }

    return a.exec();
}

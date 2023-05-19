#include <qcoreapplication.h>
#include <qtest.h>

#include <iostream>
#include <Windows.h>

#include "clientsqlitetest.h"
#include "clientmysqltest.h"
#include "clientsqlservertest.h"

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

int main(int argc, char** argv) {
    QCoreApplication a(argc, argv);

    int result = 0;
    if (TEST_DB == QLatin1String("sqlite")) {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                                FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);
        std::cout << "------------------------------ Test Sqlite ------------------------------" << std::endl;

        ClientSqliteTest sqliteTest;
        result = QTest::qExec(&sqliteTest, QStringList() << "qtdaoclienttest.exe" << "-o" << "test.txt");
        setColor();
    }

    if (TEST_DB == QLatin1String("mysql")) {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                                FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);
        std::cout << "------------------------------ Test Mysql ------------------------------" << std::endl;

        ClientMysqlTest mysqlTest;
        result += QTest::qExec(&mysqlTest, QStringList() << "qtdaoclienttest.exe" << "-o" << "test.txt");
        setColor();
    }

    if (TEST_DB == QLatin1String("sqlserver")) {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                                FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);
        std::cout << "------------------------------ Test SqlServer ------------------------------" << std::endl;

        ClientSqlServerTest sqlServerTest;
        result += QTest::qExec(&sqlServerTest, QStringList() << "qtdaoclienttest.exe" << "-o" << "test.txt");
        setColor();
    }

    if (result != 0) {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
        std::cout << "Not all tests are successful!" << std::endl;
    } else {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_GREEN);
        std::cout << "All tests passed!" << std::endl;
    }

    return result;
}
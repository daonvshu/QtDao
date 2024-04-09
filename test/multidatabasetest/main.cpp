#include <qcoreapplication.h>
#include <qtest.h>

#include <iostream>
#include <Windows.h>
#include <dao.h>

#include "multidbtest.h"

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

    int result;
    try {
        MultiDbTest tester;
        result = QTest::qExec(&tester, QStringList() << "qtdaomultidatabasetest.exe" << "-o" << "test.txt");
        setColor();
    } catch (dao::DaoException& e) {
        Q_UNUSED(e)
        auto validDrivers = QSqlDatabase::drivers();
        Q_UNUSED(validDrivers)
        qFatal("run database test fail!");
    }

    return result;
}
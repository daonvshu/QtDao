#include <QtCore/QCoreApplication>

#include <QtTest/qtest.h>

#include "test/ConnectionPoolTest.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    //test connection pool
    ConnectionPoolTest connectionPoolTest;
    QTest::qExec(&connectionPoolTest);

    return a.exec();
}

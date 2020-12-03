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

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    ConnectionPoolTest connectionPoolTest;
    QTest::qExec(&connectionPoolTest);

    BaseQueryTest baseQueryTest;
    QTest::qExec(&baseQueryTest);

    DbLoaderTest dbLoadTest;
    QTest::qExec(&dbLoadTest);

    ConnectorTest connectorTest;
    QTest::qExec(&connectorTest);

    InsertTest insertTest;
    QTest::qExec(&insertTest);

    SelectTest selectTest;
    QTest::qExec(&selectTest);

    UpdateTest updateTest;
    QTest::qExec(&updateTest);

    DeleteTest deleteTest;
    QTest::qExec(&deleteTest);

    JoinTest joinTest;
    QTest::qExec(&joinTest);

    return a.exec();
}

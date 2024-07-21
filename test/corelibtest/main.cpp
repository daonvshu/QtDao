#include <QtCore/QCoreApplication>

#include "other/ConnectionPoolTest.h"
#include "other/ConnectorTest.h"
#include "other/DbLoaderTest.h"
#include "query/BaseQueryTest.h"
#include "query/DeleteTest.h"
#include "query/InsertIntoSelectTest.h"
#include "query/InsertTest.h"
#include "query/JoinTest.h"
#include "query/LoggingTest.h"
#include "query/SelectTest.h"
#include "query/UpdateTest.h"
#include "query/UpsertTest.h"

#include "multitestrunner.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    auto testDbType = TestConfigLoader::instance().config().getTestTargetDb();
    switch (testDbType) {
        case TestTargetDb::Target_Sqlite:
            TestUtils::printWithColor("------------------------------ Test Sqlite ------------------------------", TestOutputColorAttr::Yellow);
            break;
        case TestTargetDb::Target_Mysql:
            TestUtils::printWithColor("------------------------------ Test Mysql ------------------------------", TestOutputColorAttr::Yellow);
            break;
        case TestTargetDb::Target_SqlServer:
            TestUtils::printWithColor("------------------------------ Test SqlServer ------------------------------", TestOutputColorAttr::Yellow);
            break;
        case TestTargetDb::Target_PSql:
            TestUtils::printWithColor("------------------------------ Test PSql ------------------------------", TestOutputColorAttr::Yellow);
            break;
    }

    int result = MultiTestRunner<
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
        >::run("qtdaocoretest.exe", testDbType);

    if (result != 0) {
        TestUtils::printWithColor("Not all tests are successful!", TestOutputColorAttr::Yellow);
    } else {
        TestUtils::printWithColor("All tests passed!", TestOutputColorAttr::Green);
    }
    return result;
}

#include <qcoreapplication.h>

#include "clientsqlitetest.h"
#include "clientmysqltest.h"
#include "clientsqlservertest.h"

#include "utils/testconfigloader.h"
#include "multitestrunner.h"

int main(int argc, char** argv) {
    QCoreApplication a(argc, argv);

    int result = 0;
    auto testDbType = TestConfigLoader::instance().config().getTestTargetDb();
    switch (testDbType) {
        case TestTargetDb::Target_Sqlite:
            TestUtils::printWithColor("------------------------------ Test Sqlite ------------------------------", TestOutputColorAttr::Yellow);
            result = MultiTestRunner<ClientSqliteTest>::run("qtdaoclienttest.exe");
            break;
        case TestTargetDb::Target_Mysql:
            TestUtils::printWithColor("------------------------------ Test Mysql ------------------------------", TestOutputColorAttr::Yellow);
            result = MultiTestRunner<ClientMysqlTest>::run("qtdaoclienttest.exe");
            break;
        case TestTargetDb::Target_SqlServer:
            TestUtils::printWithColor("------------------------------ Test SqlServer ------------------------------", TestOutputColorAttr::Yellow);
            result = MultiTestRunner<ClientSqlServerTest>::run("qtdaoclienttest.exe");
            break;
        case TestTargetDb::Target_PSql:
            TestUtils::printWithColor("------------------------------ Test PSql ------------------------------", TestOutputColorAttr::Yellow);
            break;
    }

    if (result != 0) {
        TestUtils::printWithColor("Not all tests are successful!", TestOutputColorAttr::Yellow);
    } else {
        TestUtils::printWithColor("All tests passed!", TestOutputColorAttr::Green);
    }
    return result;
}
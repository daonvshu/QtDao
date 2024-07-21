#include <qcoreapplication.h>

#include "upgradetest.h"
#include "multitestrunner.h"

int main(int argc, char** argv) {
    QCoreApplication a(argc, argv);

    return MultiTestRunner<UpgradeTest>::run("qtdaoupgradetest.exe");
}
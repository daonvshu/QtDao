#include <qcoreapplication.h>

#include "customtypetest.h"
#include "multitestrunner.h"

int main(int argc, char** argv) {
    QCoreApplication a(argc, argv);

    return MultiTestRunner<CustomTypeTest>::run("qtdaocustomtypetest.exe");
}
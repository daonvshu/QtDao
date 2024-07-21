#include <qcoreapplication.h>

#include "multidbtest.h"
#include "multitestrunner.h"

int main(int argc, char** argv) {
    QCoreApplication a(argc, argv);

    return MultiTestRunner<MultiDbTest>::run("qtdaomultidatabasetest.exe");
}
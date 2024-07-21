#include <qcoreapplication.h>

#include "foreignkeytest.h"
#include "multitestrunner.h"

int main(int argc, char** argv) {
    QCoreApplication a(argc, argv);

    return MultiTestRunner<ForeignKeyTest>::run("qtdaoforeignkeytest.exe");
}
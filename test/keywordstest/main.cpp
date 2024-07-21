#include <qcoreapplication.h>

#include "keywordstest.h"
#include "multitestrunner.h"

int main(int argc, char** argv) {
    QCoreApplication a(argc, argv);

    return MultiTestRunner<KeywordsTest>::run("qtdaokeywordtest.exe");
}
#include "testutils.h"

#include <qfile.h>
#include <iostream>

#ifdef Q_OS_WIN
#include <Windows.h>
#endif

#include "testconfigloader.h"

void TestUtils::printTestLog(const QString &filePath) {
    QFile file(filePath);
    Q_ASSERT(file.open(QIODevice::ReadOnly));
    while (true) {
        auto line = file.readLine();
        if (line.isEmpty()) break;
        if (line.startsWith("PASS")) {
            printWithColor(line, TestOutputColorAttr::Green, false);
        } else if (line.startsWith("FAIL")) {
            printWithColor(line, TestOutputColorAttr::Red, false);
        } else if (line.startsWith("Totals")) {
            printWithColor(line, TestOutputColorAttr::Yellow, false);
        } else if (line.startsWith("*********")) {
            printWithColor(line, TestOutputColorAttr::White, false);
        } else if (line.startsWith("QWARN")) {
            printWithColor(line, TestOutputColorAttr::Yellow, false);
        } else if (line.startsWith("QDEBUG")) {
            printWithColor(line, TestOutputColorAttr::Blue, false);
        } else {
            printWithColor(line, TestOutputColorAttr::Unset, false);
        }
    }
    printWithColor(QString(), TestOutputColorAttr::Unset);
    file.close();
}

//linux console color attribute
enum class StdColorAttr {
    LR = 31,
    LG = 32,
    LY = 33,
    LB = 34,
    LP = 35,
    LC = 36,
    LW = 37,

    //background
    LBK = 10, // x += LBK

    //lighter
    LL = 60, // x += LL

    //underline
    LU = 4,

    //blink
    LBlink = 5,
};

void TestUtils::printWithColor(const QString &data, TestOutputColorAttr colorStyle, bool end) {

    const auto& wrapLog = [&] {
        if (colorStyle == TestOutputColorAttr::Unset) {
            return data;
        }
        QString styled = "\033[";
        int v = 30 + (int)colorStyle + (int)StdColorAttr::LL;
        styled += QString::number(v) + "m";
        return styled + data + "\033[0m";
    };

    auto wrapped = wrapLog();
    const auto& printTarget = TestConfigLoader::instance().config().getOutputTarget();
    switch (printTarget) {
        case PrintOutputTarget::Target_Std: {
            if (TestConfigLoader::instance().config().systemCodePage()) {
                std::cout << wrapped.toLocal8Bit().data();
            } else {
                std::cout << wrapped.toStdString();
            }
            if (end) {
                std::cout << std::endl;
            }
        }
            break;
        case PrintOutputTarget::Target_Debug_Console: {
#ifdef Q_OS_WIN
            OutputDebugString(reinterpret_cast<const wchar_t*>(wrapped.utf16()));
            if (end) {
                OutputDebugString(L"\n");
            }
#endif
        }
            break;
    }
}

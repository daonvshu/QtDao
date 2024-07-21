#pragma once

#include <qobject.h>

enum class TestOutputColorAttr {
    Unset = 0,
    Red,
    Green,
    Yellow,
    Blue,
    Purple,
    Cyan,
    White,
};

class TestUtils {
public:
    static void printTestLog(const QString& filePath);

    static void printWithColor(const QString& data, TestOutputColorAttr colorStyle = TestOutputColorAttr::Unset, bool end = true);
};

#pragma once

#include <QtTest/QTest>

#include "utils/testutils.h"

#include <dao.h>

template<typename... Arg> struct MultiTestRunner;
template<typename T, typename... Arg>
struct MultiTestRunner<T, Arg...> : MultiTestRunner<Arg...> {

    template<typename... Params>
    static int run(const QString& target, Params... params) {
        T t(params...);
        int result;
        try {
            result = QTest::qExec(&t, QStringList() << target << "-o" << "test.txt");
            TestUtils::printTestLog("test.txt");
        } catch (dao::DaoException& e) {
            Q_UNUSED(e)
            auto validDrivers = QSqlDatabase::drivers();
            Q_UNUSED(validDrivers)
            qFatal("run testcase fail!");
        }

        result += MultiTestRunner<Arg...>::run(target, params...);
        return result;
    }
};

template<> struct MultiTestRunner<> {

    template<typename... Params>
    static int run(const QString&, Params...) { return 0; }
};
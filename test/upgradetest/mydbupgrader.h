#pragma once

#include <qobject.h>

#include "dao.h"

class MyDbUpgrader : public dao::DatabaseUpgrader {
public:
    void onUpgrade(int oldVersion, int curVersion) override;

private:
    void upgrade1To2();
    void upgrade2To3();
};

#pragma once

#include "../../global.h"

QTDAO_BEGIN_NAMESPACE

struct FromBuildData {
    QString statement;
    QVariantList values;
    QString asName;
};

class SelectImpl;
class JoinImpl;
class RecursiveQueryBuilder;

class FromBuilder {
public:
    void fromDataClear() {
        data = FromBuildData();
    }

protected:
    void from(SelectImpl& select);

    void from(JoinImpl& join);

    void fromBuilder(RecursiveQueryBuilder& builder);

private:
    FromBuildData data;
};

QTDAO_END_NAMESPACE
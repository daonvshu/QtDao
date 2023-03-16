#pragma once

#include "../../global.h"

QTDAO_BEGIN_NAMESPACE

struct UnionBuildData {
    QString statement;
    QVariantList values;
    bool unionAll = false;
};

class SelectImpl;
class JoinImpl;

class UnionBuilderImpl {
protected:
    void from(SelectImpl& select);

    void from(JoinImpl& join);

private:
    UnionBuildData data;
};

template<typename T>
class UnionBuilder : UnionBuilderImpl {
public:
    template<typename E2>
    T& unionSelect(Select<E2>& select, bool unionAll = false) {
        from(select, unionAll);
        return *this;
    }

    template<typename E2>
    T& unionSelect(Select<E2>&& select, bool unionAll = false) {
        return unionSelect(select, unionAll);
    }

    template<typename...E2>
    T& unionSelect(Join<E2...>& join, bool unionAll = false) {
        from(join, unionAll);
        return *this;
    }

    template<typename...E2>
    T& unionSelect(Join<E2...>&& join, bool unionAll = false) {
        return unionSelect(join, unionAll);
    }
};

QTDAO_END_NAMESPACE
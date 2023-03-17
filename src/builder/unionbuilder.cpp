#include "builder/option/unionbuilder.h"

#include "query/selectimpl.h"
#include "query/joinimpl.h"

QTDAO_BEGIN_NAMESPACE

void dao::UnionBuilderImpl::unionWithSelect(SelectImpl &select, bool unionAll) {
    select.buildFilterSqlStatement();
    unionData.statement = select.statement;
    unionData.values = select.values;
    unionData.unionAll = unionAll;
}

void UnionBuilderImpl::unionWithJoin(JoinImpl &join, bool unionAll) {
    join.buildJoinSqlStatement();
    unionData.statement = join.statement;
    unionData.values = join.values;
    unionData.unionAll = unionAll;
}

QTDAO_END_NAMESPACE


#include "builder/option/unionbuilder.h"

#include "query/selectimpl.h"
#include "query/joinimpl.h"

QTDAO_BEGIN_NAMESPACE

void dao::UnionBuilderImpl::from(SelectImpl &select) {
    select.buildFilterSqlStatement();
    unionSelectStatement = select.statement;
    unionSelectValues = select.values;
    this->unionAll = unionAll;
}

void UnionBuilderImpl::from(JoinImpl &join) {
    join.buildJoinSqlStatement();
    unionSelectStatement = join.statement;
    unionSelectValues = join.values;
    this->unionAll = unionAll;
}

QTDAO_END_NAMESPACE


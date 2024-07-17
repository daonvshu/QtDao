#include "builder/option/unionbuilder.h"

#include "query/selectimpl.h"
#include "query/joinimpl.h"

#if QT_VERSION_MAJOR >= 6
#include "config/configbuilder.h"
#include "config/configmanager.h"

QTDAO_BEGIN_NAMESPACE

void UnionBuilderImpl::unionWithSelect(SelectImpl &select, bool unionAll) {
    auto config = ConfigManager::getConfig();
    if (config->isSqlServer()) {
        qFatal("The current version of the union query in Qt6 fails the test case when using SQLServer.");
    }
    select.buildFilterSqlStatement();
    unionData.statement = select.statement;
    unionData.values = select.values;
    unionData.unionAll = unionAll;
}

void UnionBuilderImpl::unionWithJoin(JoinImpl &join, bool unionAll) {
    auto config = ConfigManager::getConfig();
    if (config->isSqlServer()) {
        qFatal("The current version of the union query in Qt6 fails the test case when using SQLServer.");
    }
    join.buildJoinSqlStatement();
    unionData.statement = join.statement;
    unionData.values = join.values;
    unionData.unionAll = unionAll;
}
#else

QTDAO_BEGIN_NAMESPACE

void UnionBuilderImpl::unionWithSelect(SelectImpl &select, bool unionAll) {
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
#endif

QTDAO_END_NAMESPACE


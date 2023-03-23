#pragma once

#include "../query/join.h"

#include "option/debugbuilder.h"
#include "option/columnbuilder.h"
#include "option/filterbuilder.h"
#include "option/onconditionbuilder.h"
#include "option/constraintbuilder.h"
#include "option/unionbuilder.h"
#include "option/joinconnectbuilder.h"

#include "../macro/macro.h"

QTDAO_BEGIN_NAMESPACE

template<typename... E>
class JoinBuilder
        : public DebugBuilder<JoinBuilder<E...>>
        , public ColumnBuilder<JoinBuilder<E...>>
        , public FilterBuilder<JoinBuilder<E...>>
        , public OnConditionBuilder<JoinBuilder<E...>>
        , public ConstraintBuilder<JoinBuilder<E...>>
        , public UnionBuilder<JoinBuilder<E...>>
        , public JoinConnectBuilder<JoinBuilder<E...>>
        , public FromEsSelectBuilder<JoinBuilder, E...>
{
public:
    using FromEsSelectBuilder<JoinBuilder, E...>::from;

    template<typename E2>
    JoinBuilder<E...>& from();

    Join<E...> build();

protected:
    void doFilter() override;

private:
    QString mainTable;
    QHash<QString, JoinData> subJoinData;
};

template<typename ...E>
inline void JoinBuilder<E...>::doFilter() {
    JoinData data;
    data.joinType = this->joinType;
    data.filter = OnConditionBuilder<JoinBuilder<E...>>::filterCondition;
    data.fromBuildData = this->JoinConnectBuilder<JoinBuilder<E...>>::fromData;
    subJoinData.insert(this->tbName, data);
    OnConditionBuilder<JoinBuilder<E...>>::filterCondition = FilterGroupConnector();
    this->JoinConnectBuilder<JoinBuilder<E...>>::fromDataClear();
}

template<typename ...E>
template<typename E2>
inline JoinBuilder<E...>& JoinBuilder<E...>::from() {
    mainTable = E2::Info::getTableName();
    return *this;
}

template<typename ...E>
inline Join<E...> JoinBuilder<E...>::build() {
    if (mainTable.isEmpty()) {
        mainTable = this->FromEsSelectBuilder<JoinBuilder, E...>::fromData.asName;
    }
    return Join<E...>(mainTable, subJoinData, *this);
}

QTDAO_END_NAMESPACE
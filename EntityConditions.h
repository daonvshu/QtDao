#pragma once

#define and &&
#define or ||

#include <qobject.h>
#include "EntityConditionInterface.h"

class EntityField;
class EntityFunction;
class EntityConditions : public EntityConditionInterface {

public:
    EntityConditions& operator&&(const EntityConditions& oth);
    EntityConditions& operator&&(const EntityFunction& oth);

    EntityConditions& operator||(const EntityConditions& oth);
    EntityConditions& operator||(const EntityFunction& oth);

    EntityConditions& operator,(const EntityConditions& oth);
    EntityConditions& operator,(const EntityFunction& oth); 
    EntityConditions& operator,(const EntityField& oth);
        
    QString getExpressionStr(bool withoutCombineOp = false) const override;
    QVariantList getValueList() const override;
    bool isAsc() const override;
    QStringList getBindFields(bool withoutJoinPrefix = false) const;

    void clearAll();

private:
    QList<EntityField> entityFields;

private:
    EntityConditions& addField(const EntityField& field);

    EntityConditions& operator()();

    friend class EntityField;
    friend class EntityFunction;
}; 
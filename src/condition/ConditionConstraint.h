#pragma once

class ConditionConstraint {
public:
    static ConditionConstraint limit(int a, int b);
    static ConditionConstraint limit(int a);

    static ConditionConstraint orderBy();

    static ConditionConstraint groupBy();

    static ConditionConstraint having();
};
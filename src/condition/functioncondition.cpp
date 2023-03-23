#include "condition/functioncondition.h"

QTDAO_BEGIN_NAMESPACE

QList<FieldInfo> FunctionConnector::getUsedFields() {
    int asIndex = expressions.indexOf(" as ");
    if (asIndex != -1) {
        if (fields.isEmpty()) {
            return QList<FieldInfo>() << FieldInfo{ expressions.mid(asIndex + 4), "" }; //empty tablename
        } else {
            return QList<FieldInfo>() << FieldInfo{ expressions.mid(asIndex + 4), fields.at(0).bindTable }; //use first bind table
        }
    }
    if (fields.isEmpty()) {
        return QList<FieldInfo>() << FieldInfo{ expressions, "" }; //empty tablename
    }
    QString str = expressions;
    for (int i = 0; i < fields.size(); i++) {
        str = str.arg(getField(i));
    }
    return QList<FieldInfo>() << FieldInfo{ str, fields.at(0).bindTable }; //use first bind table
}

void FunctionConnector::combine() {
    QString str = expressions;
    for (int i = 0; i < fields.size(); i++) {
        str = str.arg(getField(i));
    }
    connectedStr = str;
}

void FunctionConnector::fromSelect(SelectImpl& select) {
    FromBuilder::fromSelect(select);
    solveFromQueryBuildResult();
}

void FunctionConnector::fromJoin(JoinImpl& join) {
    FromBuilder::fromJoin(join);
    solveFromQueryBuildResult();
}

void FunctionConnector::fromBuilder(RecursiveQueryBuilder& builder) {
    FromBuilder::fromBuilder(builder);
    solveFromQueryBuildResult();
}

void FunctionConnector::solveFromQueryBuildResult() {
    fields << FieldInfo{ fromData.statement.prepend('(').append(')'), "" };
    values << fromData.values;
    fromDataClear();
}

QTDAO_END_NAMESPACE
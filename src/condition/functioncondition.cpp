#include "condition/functioncondition.h"

QTDAO_BEGIN_NAMESPACE

QList<FieldInfo> FunctionConnector::getUsedFields() {
    int asIndex = expressions.indexOf(" as ");
    if (asIndex != -1) {
        if (d->fields.isEmpty()) {
            return QList<FieldInfo>() << FieldInfo{ expressions.mid(asIndex + 4), "" }; //empty tablename
        } else {
            return QList<FieldInfo>() << FieldInfo{ expressions.mid(asIndex + 4), d->fields.at(0).bindTable }; //use first bind table
        }
    }
    if (d->fields.isEmpty()) {
        return QList<FieldInfo>() << FieldInfo{ expressions, "" }; //empty tablename
    }
    QString str = expressions;
    for (int i = 0; i < d->fields.size(); i++) {
        str = str.arg(getField(i));
    }
    return QList<FieldInfo>() << FieldInfo{ str, d->fields.at(0).bindTable }; //use first bind table
}

void FunctionConnector::combine() {
    QString str = expressions;
    for (int i = 0; i < d->fields.size(); i++) {
        str = str.arg(getField(i));
    }
    d->connectedStr = str;
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
    d->fields << FieldInfo{ fromData.statement.prepend('(').append(')'), "" };
    d->values << fromData.values;
    fromDataClear();
}

bool FunctionConnector::isEmpty() {
    return expressions.isEmpty() && Connectable::isEmpty();
}

QTDAO_END_NAMESPACE
#pragma once

#include "../global.h"

#include <qobject.h>
#include <qvariant.h>

#include "utils/serializing.h"

#include "connector/entityconnector.h"

QTDAO_BEGIN_NAMESPACE

class OperatorEntityConnector : EntityConnector {
public:
    using EntityConnector::addField;
    using EntityConnector::addValue;
    using EntityConnector::addCustomValue;
    using EntityConnector::ptr;

    void setOperator(const QString& op) {
        this->connectorOp = op;
    }

    void combine() override;

protected:
    QString connectorOp;

    using EntityConnector::getField;
    using EntityConnector::connectedStr;
};

class SelfOperatorEntityConnector : public OperatorEntityConnector {
public:
    void combine() override;
};

template<typename T, typename Connector>
class BatchOperatorEntityConnectorImpl : Connector {
public:
    using Connector::setOperator;
    using Connector::addField;
    using Connector::ptr;

    void addValues(const QList<T>& values) {
        QVariantList variantList;
        for (const auto& v : values) {
            variantList << v;
        }
        Connector::addValue(QVariant(variantList));
    }

    void addCustomValues(const QList<T>& values) {
        QVariantList variantList;
        for (const auto& v : values) {
            variantList << serializeCustomTypeToBinary(v);
        }
        Connector::addValue(QVariant(variantList));
    }
};

template<typename T>
class BatchOperatorEntityConnector : public BatchOperatorEntityConnectorImpl<T, OperatorEntityConnector> {};

template<typename T>
class BatchSelfOperatorEntityConnector : public BatchOperatorEntityConnectorImpl<T, SelfOperatorEntityConnector> {};

class FieldOperatorEntityConnector : EntityConnector {
public:
    using EntityConnector::ptr;

    void addFields(const FieldInfo& to, const FieldInfo& from) {
        addField(to);
        addField(from);
    }

    void setOperator(const QString& op) {
        this->connectorOp = op;
    }

    void combine() override;

protected:
    QString connectorOp;

    using EntityConnector::getField;
    using EntityConnector::connectedStr;
};

class SelfFieldOperatorEntityConnector : public FieldOperatorEntityConnector {
public:
    void combine() override;
};

class InEntityConnector : EntityConnector {
public:
    using EntityConnector::addField;
    using EntityConnector::ptr;

    template<typename T>
    void addValues(const QList<T>& values) {
        for (const auto& v : values) {
            addValue(v);
        }
    }

    template<typename T>
    void addCustomValues(const QList<T>& values) {
        for (const auto& v : values) {
            addValue(serializeCustomTypeToBinary(v));
        }
    }

    void combine() override;
};

class BetweenConnector : EntityConnector {
public:
    using EntityConnector::addField;
    using EntityConnector::ptr;

    template<typename T>
    void addValues(const T& from, const T& to) {
        addValue(from);
        addValue(to);
    }

    void combine() override;
};

class IsNullConnector : EntityConnector {
public:
    explicit IsNullConnector(bool isNull) : checkIsNull(isNull) {}

    using EntityConnector::addField;
    using EntityConnector::ptr;

    void combine() override;

private:
    bool checkIsNull;
};

QTDAO_END_NAMESPACE
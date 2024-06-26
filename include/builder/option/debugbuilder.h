#pragma once

#include "../../global.h"

#include "../../utils/logging.h"

QTDAO_BEGIN_NAMESPACE

template<typename T>
class DebugBuilder {
public:
    /**
     * set current query logging category, call 'dao::loggingUseDefault' to use default category 'qtdao.query'
     * @param ptr new logging category function pointer
     * @return this
     */
    T& logging(LoggingCategoryPtr ptr) {
        loggingCategoryPtr = ptr;
        return static_cast<T&>(*this);
    }

protected:
    //print sql query log use target category
    LoggingCategoryPtr loggingCategoryPtr = nullptr;

    friend class BaseQuery;
};

QTDAO_END_NAMESPACE
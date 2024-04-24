#pragma once

#include "dao.version.h"

#define QTDAO_BEGIN_NAMESPACE namespace dao {

#define QTDAO_END_NAMESPACE }

#define QTDAO_USING_NAMESPACE using namespace dao;

#define QTDAO_VERSION_CHECK(major, minor, patch) ((major<<16)|(minor<<8)|(patch))
#define QTDAO_VERSION QTDAO_VERSION_CHECK(QTDAO_VERSION_MAJOR, QTDAO_VERSION_MINOR, QTDAO_VERSION_PATCH)

//#define QTDAO_DEBUG

#ifdef QTDAO_DEBUG
#include <iostream>
#include <qdebug.h>

//#define DEBUG_USE_STD
#ifdef DEBUG_USE_STD
#define PRINT_DEBUGGER std::cout
#else
#define PRINT_DEBUGGER qDebug()
#endif

#define MESSAGE_DEBUG(...)    messageDebug(PRINT_DEBUGGER, __VA_ARGS__)

QTDAO_BEGIN_NAMESPACE

template<typename D, typename T>
inline void printMessage(D& debug, const T& msg) {
    debug << msg;
}

template<>
inline void printMessage(decltype(std::cout)& debug, const QString& msg) {
    debug << msg.toStdString();
}

template<typename D, typename T, typename... Args>
inline void messageDebug(D& debug, const T& msg, const Args&... args) {
    printMessage(debug, msg);
    messageDebug(debug, args...);
}

template<typename D>
inline void messageDebug(D&) {}

template<>
inline void messageDebug(decltype(std::cout)& debug) {
    debug << std::endl;
}

QTDAO_END_NAMESPACE

#else
#define MESSAGE_DEBUG(...)
#endif
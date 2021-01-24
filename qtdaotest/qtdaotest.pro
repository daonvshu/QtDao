QT += testlib sql
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

INCLUDEPATH += ../include

CONFIG(debug, debug|release){
    contains(DEFINES, WIN64){
        LIBS += -L$$PWD/../lib/x64 -lqtdao_d
    } else {
        LIBS += -L$$PWD/../lib/x86 -lqtdao_d
    }
} else {
    contains(DEFINES, WIN64){
        LIBS += -L$$PWD/../lib/x64 -lqtdao
    } else {
        LIBS += -L$$PWD/../lib/x86 -lqtdao
    }
}

include(qtdaotest.pri)

DEFINES += QT_DAO_TESTCASE

QT -= gui
QT += sql

TEMPLATE = lib
CONFIG += staticlib

CONFIG += c++11

CONFIG += debug_and_release build_all

CONFIG(debug, debug|release){
    contains(DEFINES, WIN64){
        DESTDIR = $$PWD/../lib/x64
    } else {
        DESTDIR = $$PWD/../lib/x86
    }
    TARGET = qtdao_d
} else {
    contains(DEFINES, WIN64){
        DESTDIR = $$PWD/../lib/x64
    } else {
        DESTDIR = $$PWD/../lib/x86
    }
    TARGET = qtdao
}

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

include(qtdaolib.pri)

INCLUDEPATH += ../include

# Default rules for deployment.
unix {
    target.path = $$[QT_INSTALL_PLUGINS]/generic
}
!isEmpty(target.path): INSTALLS += target


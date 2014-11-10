#-------------------------------------------------
#
# Project created by QtCreator 2014-07-31T11:30:01
#
#-------------------------------------------------

QT -= gui
QT += network
QT += sql

DESTDIR     = $$PWD/../../BUILD/core
OBJECTS_DIR = $$PWD/../../BUILD/core/obj
UI_DIR      = $$PWD/../../BUILD/core/ui
MOC_DIR     = $$PWD/../../BUILD/core/moc
RCC_DIR     = $$PWD/../../BUILD/core/rcc

TARGET = core
TEMPLATE = lib
CONFIG += staticlib

SOURCES += \
    sources/dao_service.cpp \
    sources/vcomwrapper.cpp \
    sources/idkitwrapper.cpp \
    sources/printerserial.cpp \
    sources/soaphandler.cpp \
    sources/dbaccess.cpp \
    sources/biobase64.cpp \
    sources/utils.cpp \
    sources/networkmonitor.cpp

HEADERS += \
    headers/dao_service.h \
    headers/vcomwrapper.h \
    headers/idkitwrapper.h \
    headers/printerserial.h \
    headers/soaphandler.h \
    headers/dbaccess.h \
    headers/biobase64.h \
    headers/utils.h \
    headers/networkmonitor.h

OTHER_FILES += \
    core_updater.pri \
    core_tempo.pri \
    core_synchronizer.pri \
    core_backup.pri

INCLUDEPATH += $$PWD/headers
INCLUDEPATH += $$PWD/../includes

CONFIG(release, debug|release) {
  DEFINES += NDEBUG NTRACE_BILDERBIKKEL
}

QMAKE_CXXFLAGS += -Wall -Wextra
unix {
  QMAKE_CXXFLAGS += -Werror
}

include($$PWD/../../LIB/qextserialport-1.2rc/src/qextserialport.pri)

LIBS += -L$$PWD/../../LIB/vcom/lib -lVCOMEx
INCLUDEPATH += $$PWD/../../LIB/vcom/include
DEPENDPATH += $$PWD/../../LIB/vcom/include

LIBS += -L$$PWD/../../LIB/idkit/lib/ -lidkit
INCLUDEPATH += $$PWD/../../LIB/idkit/include
DEPENDPATH += $$PWD/../../LIB/idkit/include$

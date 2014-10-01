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
    sources/vcomwrapper.cpp \
    sources/idkitwrapper.cpp \
    sources/printerserial.cpp \
    sources/soaphandler.cpp \
    sources/dbaccess.cpp \
    sources/biobase64.cpp \
    sources/utils.cpp \
    sources/networkmonitor.cpp

HEADERS += \
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
    core_synchronizer.pri

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

contains(DEFINES, TARGET) {
  LIBS += -L$$PWD/../../LIB/vcom_target/lib -lVCOMEx
  INCLUDEPATH += $$PWD/../../LIB/vcom_target/include
  DEPENDPATH += $$PWD/../../LIB/vcom_target/include

  LIBS += -L$$PWD/../../LIB/idkit_target/lib/ -lidkit
  INCLUDEPATH += $$PWD/../../LIB/idkit_target/include
  DEPENDPATH += $$PWD/../../LIB/idkit_target/include$
} else {
  LIBS += -L$$PWD/../../LIB/vcom_host/lib -lVCOMEx
  INCLUDEPATH += $$PWD/../../LIB/vcom_host/include
  DEPENDPATH += $$PWD/../../LIB/vcom_host/include

  LIBS += -L$$PWD/../../LIB/idkit_host/lib/ -lidkit
  INCLUDEPATH += $$PWD/../../LIB/idkit_host/include
  DEPENDPATH += $$PWD/../../LIB/idkit_host/include$
}

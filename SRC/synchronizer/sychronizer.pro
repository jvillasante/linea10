#-------------------------------------------------
#
# Project created by QtCreator 2014-08-25T10:16:09
#
#-------------------------------------------------

QT += core
QT += sql
QT += network
QT -= gui

DESTDIR     = $$PWD/../../BUILD/synchronizer
OBJECTS_DIR = $$PWD/../../BUILD/synchronizer/obj
UI_DIR      = $$PWD/../../BUILD/synchronizer/ui
MOC_DIR     = $$PWD/../../BUILD/synchronizer/moc
RCC_DIR     = $$PWD/../../BUILD/synchronizer/rcc

TARGET = synchronizer
TEMPLATE = app

CONFIG += console
CONFIG -= app_bundle

HEADERS += \
    headers/synchronizer.h

SOURCES += \
    sources/main.cpp \
    sources/synchronizer.cpp

INCLUDEPATH += $$PWD/headers
INCLUDEPATH += $$PWD/../includes

CONFIG(release, debug|release) {
  DEFINES += NDEBUG NTRACE_BILDERBIKKEL
}

QMAKE_CXXFLAGS += -Wall -Wextra
unix {
  QMAKE_CXXFLAGS += -Werror
}

include($$PWD/../core/core_synchronizer.pri)

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


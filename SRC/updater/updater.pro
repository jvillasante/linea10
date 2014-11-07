#-------------------------------------------------
#
# Project created by QtCreator 2014-08-25T10:16:09
#
#-------------------------------------------------

QT += core
QT += sql
QT += network
QT -= gui

DESTDIR     = $$PWD/../../BUILD/updater
OBJECTS_DIR = $$PWD/../../BUILD/updater/obj
UI_DIR      = $$PWD/../../BUILD/updater/ui
MOC_DIR     = $$PWD/../../BUILD/updater/moc
RCC_DIR     = $$PWD/../../BUILD/updater/rcc

TARGET = updater
TEMPLATE = app

CONFIG += console
CONFIG -= app_bundle

HEADERS += \
    headers/firmwareupdater.h

SOURCES += \
    sources/main.cpp \
    sources/firmwareupdater.cpp

INCLUDEPATH += $$PWD/headers
INCLUDEPATH += $$PWD/../includes

CONFIG(release, debug|release) {
  DEFINES += NDEBUG NTRACE_BILDERBIKKEL
}

QMAKE_CXXFLAGS += -Wall -Wextra
unix {
  QMAKE_CXXFLAGS += -Werror
}

include($$PWD/../core/core_updater.pri)

LIBS += -L$$PWD/../../LIB/vcom/lib -lVCOMEx
INCLUDEPATH += $$PWD/../../LIB/vcom/include
DEPENDPATH += $$PWD/../../LIB/vcom/include

LIBS += -L$$PWD/../../LIB/idkit/lib/ -lidkit
INCLUDEPATH += $$PWD/../../LIB/idkit/include
DEPENDPATH += $$PWD/../../LIB/idkit/include$

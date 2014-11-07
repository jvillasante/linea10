#-------------------------------------------------
#
# Project created by QtCreator 2014-08-25T10:16:09
#
#-------------------------------------------------

QT += core
QT += sql
QT -= gui

DESTDIR     = $$PWD/../../BUILD/backup
OBJECTS_DIR = $$PWD/../../BUILD/backup/obj
UI_DIR      = $$PWD/../../BUILD/backup/ui
MOC_DIR     = $$PWD/../../BUILD/backup/moc
RCC_DIR     = $$PWD/../../BUILD/backup/rcc

TARGET = backup
TEMPLATE = app

CONFIG += console
CONFIG -= app_bundle

HEADERS += \
    headers/backup.h

SOURCES += \
    sources/main.cpp \
    sources/backup.cpp

INCLUDEPATH += $$PWD/headers
INCLUDEPATH += $$PWD/../includes

CONFIG(release, debug|release) {
  DEFINES += NDEBUG NTRACE_BILDERBIKKEL
}

QMAKE_CXXFLAGS += -Wall -Wextra
unix {
  QMAKE_CXXFLAGS += -Werror
}

include($$PWD/../core/core_backup.pri)

LIBS += -L$$PWD/../../LIB/vcom/lib -lVCOMEx
INCLUDEPATH += $$PWD/../../LIB/vcom/include
DEPENDPATH += $$PWD/../../LIB/vcom/include

LIBS += -L$$PWD/../../LIB/idkit/lib/ -lidkit
INCLUDEPATH += $$PWD/../../LIB/idkit/include
DEPENDPATH += $$PWD/../../LIB/idkit/include$


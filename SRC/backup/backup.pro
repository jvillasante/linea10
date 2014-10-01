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


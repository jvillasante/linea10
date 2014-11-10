#-------------------------------------------------
#
# Project created by QtCreator 2014-07-31T11:23:58
#
#-------------------------------------------------

QT += core
QT += gui
QT += xml
QT += sql
QT += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

DESTDIR     = $$PWD/../../BUILD/snack
OBJECTS_DIR = $$PWD/../../BUILD/snack/obj
UI_DIR      = $$PWD/../../BUILD/snack/ui
MOC_DIR     = $$PWD/../../BUILD/snack/moc
RCC_DIR     = $$PWD/../../BUILD/snack/rcc

TARGET   = snack
TEMPLATE = app

HEADERS  += \
    headers/mainwindow.h \
    headers/workersensormulti.h \
    headers/workerenroller.h \
    headers/enrolldialog.h

SOURCES += \
    sources/main.cpp\
    sources/mainwindow.cpp \
    sources/workersensormulti.cpp \
    sources/workerenroller.cpp \
    sources/enrolldialog.cpp

RESOURCES += \
    resources.qrc

TRANSLATIONS = Resources/languages/snack_es.ts \
               Resources/languages/snack_en.ts
CODECFORTR = UTF-8

INCLUDEPATH += $$PWD/headers
INCLUDEPATH += $$PWD/../includes

CONFIG(release, debug|release) {
  DEFINES += NDEBUG NTRACE_BILDERBIKKEL
}

QMAKE_CXXFLAGS += -Wall -Wextra
unix {
  QMAKE_CXXFLAGS += -Werror
}

include($$PWD/../core/core_snack.pri)
include($$PWD/../../LIB/qextserialport-1.2rc/src/qextserialport.pri)

LIBS += -L$$PWD/../../LIB/vcom/lib -lVCOMEx
INCLUDEPATH += $$PWD/../../LIB/vcom/include
DEPENDPATH += $$PWD/../../LIB/vcom/include

LIBS += -L$$PWD/../../LIB/idkit/lib/ -lidkit
INCLUDEPATH += $$PWD/../../LIB/idkit/include
DEPENDPATH += $$PWD/../../LIB/idkit/include$

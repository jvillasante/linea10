INCLUDEPATH += $$PWD
INCLUDEPATH += $$PWD/headers
DEPENDPATH += $$PWD

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

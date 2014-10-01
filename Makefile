#############################################################################
# Makefile for building: BUILD/synchronizer/synchronizer
# Generated by qmake (2.01a) (Qt 4.7.2) on: mar sep 30 13:25:54 2014
# Project:  SRC/synchronizer/sychronizer.pro
# Template: app
# Command: /home/jvillasante/hacking_lily/Develop/build/qt-embedded-4.7.2/TARGET/bin/qmake -spec ../../../Develop/build/qt-embedded-4.7.2/TARGET/mkspecs/qws/linux-arm-gnueabi-g++ CONFIG+=release DEFINES+=TARGET -o Makefile SRC/synchronizer/sychronizer.pro
#############################################################################

####### Compiler, tools and options

CC            = arm-none-linux-gnueabi-gcc
CXX           = arm-none-linux-gnueabi-g++
DEFINES       = -DTARGET -DNDEBUG -DNTRACE_BILDERBIKKEL -DQT_NO_DEBUG -DQT_SQL_LIB -DQT_NETWORK_LIB -DQT_CORE_LIB -DQT_SHARED
CFLAGS        = -pipe -Wno-psabi -Os -march=armv5te -Wall -W -D_REENTRANT $(DEFINES)
CXXFLAGS      = -pipe -Wall -Wextra -Werror -Wno-psabi -Os -march=armv5te -Wall -W -D_REENTRANT $(DEFINES)
INCPATH       = -I../../../Develop/build/qt-embedded-4.7.2/TARGET/mkspecs/qws/linux-arm-gnueabi-g++ -ISRC/synchronizer -I../../../Develop/build/qt-embedded-4.7.2/TARGET/include/QtCore -I../../../Develop/build/qt-embedded-4.7.2/TARGET/include/QtNetwork -I../../../Develop/build/qt-embedded-4.7.2/TARGET/include/QtSql -I../../../Develop/build/qt-embedded-4.7.2/TARGET/include -ISRC/synchronizer/headers -ISRC/includes -ISRC/core -ISRC/core/headers -ILIB/vcom_target/include -ILIB/idkit_target/include -IBUILD/synchronizer/moc -I../../../Develop/build/tslib/include -I.
LINK          = arm-none-linux-gnueabi-g++
LFLAGS        = -Wl,-O1 -Wl,-rpath,/home/jvillasante/hacking_lily/Develop/build/qt-embedded-4.7.2/TARGET/lib
LIBS          = $(SUBLIBS)  -L/home/jvillasante/hacking_lily/Develop/build/tslib/lib -L/home/jvillasante/hacking_lily/Develop/build/qt-embedded-4.7.2/TARGET/lib -L/home/jvillasante/hacking_lily/workspace/work/linea10/SRC/synchronizer/../../LIB/vcom_target/lib -lVCOMEx -L/home/jvillasante/hacking_lily/workspace/work/linea10/SRC/synchronizer/../../LIB/idkit_target/lib/ -lidkit -lQtSql -L/home/jvillasante/hacking_lily/Develop/build/qt-embedded-4.7.2/TARGET/lib -L/home/jvillasante/hacking_lily/Develop/build/tslib/lib -lQtNetwork -lQtCore -lts -lpthread 
AR            = arm-none-linux-gnueabi-ar cqs
RANLIB        = 
QMAKE         = /home/jvillasante/hacking_lily/Develop/build/qt-embedded-4.7.2/TARGET/bin/qmake
TAR           = tar -cf
COMPRESS      = gzip -9f
COPY          = cp -f
SED           = sed
COPY_FILE     = $(COPY)
COPY_DIR      = $(COPY) -r
STRIP         = arm-none-linux-gnueabi-strip
INSTALL_FILE  = install -m 644 -p
INSTALL_DIR   = $(COPY_DIR)
INSTALL_PROGRAM = install -m 755 -p
DEL_FILE      = rm -f
SYMLINK       = ln -f -s
DEL_DIR       = rmdir
MOVE          = mv -f
CHK_DIR_EXISTS= test -d
MKDIR         = mkdir -p

####### Output directory

OBJECTS_DIR   = BUILD/synchronizer/obj/

####### Files

SOURCES       = SRC/synchronizer/sources/main.cpp \
		SRC/synchronizer/sources/synchronizer.cpp \
		SRC/core/sources/soaphandler.cpp \
		SRC/core/sources/dbaccess.cpp \
		SRC/core/sources/idkitwrapper.cpp \
		SRC/core/sources/utils.cpp BUILD/synchronizer/moc/moc_synchronizer.cpp \
		BUILD/synchronizer/moc/moc_soaphandler.cpp \
		BUILD/synchronizer/moc/moc_dbaccess.cpp
OBJECTS       = BUILD/synchronizer/obj/main.o \
		BUILD/synchronizer/obj/synchronizer.o \
		BUILD/synchronizer/obj/soaphandler.o \
		BUILD/synchronizer/obj/dbaccess.o \
		BUILD/synchronizer/obj/idkitwrapper.o \
		BUILD/synchronizer/obj/utils.o \
		BUILD/synchronizer/obj/moc_synchronizer.o \
		BUILD/synchronizer/obj/moc_soaphandler.o \
		BUILD/synchronizer/obj/moc_dbaccess.o
DIST          = ../Develop/build/qt-embedded-4.7.2/TARGET/mkspecs/common/g++.conf \
		../Develop/build/qt-embedded-4.7.2/TARGET/mkspecs/common/unix.conf \
		../Develop/build/qt-embedded-4.7.2/TARGET/mkspecs/common/linux.conf \
		../Develop/build/qt-embedded-4.7.2/TARGET/mkspecs/common/qws.conf \
		../Develop/build/qt-embedded-4.7.2/TARGET/mkspecs/qconfig.pri \
		../Develop/build/qt-embedded-4.7.2/TARGET/mkspecs/features/qt_functions.prf \
		../Develop/build/qt-embedded-4.7.2/TARGET/mkspecs/features/qt_config.prf \
		../Develop/build/qt-embedded-4.7.2/TARGET/mkspecs/features/exclusive_builds.prf \
		../Develop/build/qt-embedded-4.7.2/TARGET/mkspecs/features/default_pre.prf \
		SRC/synchronizer/SRC/core/core_synchronizer.pri \
		../Develop/build/qt-embedded-4.7.2/TARGET/mkspecs/features/release.prf \
		../Develop/build/qt-embedded-4.7.2/TARGET/mkspecs/features/default_post.prf \
		../Develop/build/qt-embedded-4.7.2/TARGET/mkspecs/features/warn_on.prf \
		../Develop/build/qt-embedded-4.7.2/TARGET/mkspecs/features/qt.prf \
		../Develop/build/qt-embedded-4.7.2/TARGET/mkspecs/features/unix/thread.prf \
		../Develop/build/qt-embedded-4.7.2/TARGET/mkspecs/features/moc.prf \
		../Develop/build/qt-embedded-4.7.2/TARGET/mkspecs/features/resources.prf \
		../Develop/build/qt-embedded-4.7.2/TARGET/mkspecs/features/uic.prf \
		../Develop/build/qt-embedded-4.7.2/TARGET/mkspecs/features/yacc.prf \
		../Develop/build/qt-embedded-4.7.2/TARGET/mkspecs/features/lex.prf \
		SRC/synchronizer/sychronizer.pro
QMAKE_TARGET  = synchronizer
DESTDIR       = BUILD/synchronizer/
TARGET        = BUILD/synchronizer/synchronizer

first: all
####### Implicit rules

.SUFFIXES: .o .c .cpp .cc .cxx .C

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cc.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cxx.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.C.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.c.o:
	$(CC) -c $(CFLAGS) $(INCPATH) -o "$@" "$<"

####### Build rules

all: Makefile $(TARGET)

$(TARGET):  $(OBJECTS)  
	@$(CHK_DIR_EXISTS) BUILD/synchronizer/ || $(MKDIR) BUILD/synchronizer/ 
	$(LINK) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(OBJCOMP) $(LIBS)

Makefile: SRC/synchronizer/sychronizer.pro  ../../../Develop/build/qt-embedded-4.7.2/TARGET/mkspecs/common/g++.conf \
		../../../Develop/build/qt-embedded-4.7.2/TARGET/mkspecs/common/unix.conf \
		../../../Develop/build/qt-embedded-4.7.2/TARGET/mkspecs/common/linux.conf \
		../../../Develop/build/qt-embedded-4.7.2/TARGET/mkspecs/common/qws.conf \
		../../../Develop/build/qt-embedded-4.7.2/TARGET/mkspecs/qconfig.pri \
		../../../Develop/build/qt-embedded-4.7.2/TARGET/mkspecs/features/qt_functions.prf \
		../../../Develop/build/qt-embedded-4.7.2/TARGET/mkspecs/features/qt_config.prf \
		../../../Develop/build/qt-embedded-4.7.2/TARGET/mkspecs/features/exclusive_builds.prf \
		../../../Develop/build/qt-embedded-4.7.2/TARGET/mkspecs/features/default_pre.prf \
		SRC/core/core_synchronizer.pri \
		../../../Develop/build/qt-embedded-4.7.2/TARGET/mkspecs/features/release.prf \
		../../../Develop/build/qt-embedded-4.7.2/TARGET/mkspecs/features/default_post.prf \
		../../../Develop/build/qt-embedded-4.7.2/TARGET/mkspecs/features/warn_on.prf \
		../../../Develop/build/qt-embedded-4.7.2/TARGET/mkspecs/features/qt.prf \
		../../../Develop/build/qt-embedded-4.7.2/TARGET/mkspecs/features/unix/thread.prf \
		../../../Develop/build/qt-embedded-4.7.2/TARGET/mkspecs/features/moc.prf \
		../../../Develop/build/qt-embedded-4.7.2/TARGET/mkspecs/features/resources.prf \
		../../../Develop/build/qt-embedded-4.7.2/TARGET/mkspecs/features/uic.prf \
		../../../Develop/build/qt-embedded-4.7.2/TARGET/mkspecs/features/yacc.prf \
		../../../Develop/build/qt-embedded-4.7.2/TARGET/mkspecs/features/lex.prf \
		/home/jvillasante/hacking_lily/Develop/build/qt-embedded-4.7.2/TARGET/lib/libQtSql.prl \
		/home/jvillasante/hacking_lily/Develop/build/qt-embedded-4.7.2/TARGET/lib/libQtCore.prl \
		/home/jvillasante/hacking_lily/Develop/build/qt-embedded-4.7.2/TARGET/lib/libQtNetwork.prl
	$(QMAKE) -spec ../../../Develop/build/qt-embedded-4.7.2/TARGET/mkspecs/qws/linux-arm-gnueabi-g++ CONFIG+=release DEFINES+=TARGET -o Makefile SRC/synchronizer/sychronizer.pro
../../../Develop/build/qt-embedded-4.7.2/TARGET/mkspecs/common/g++.conf:
../../../Develop/build/qt-embedded-4.7.2/TARGET/mkspecs/common/unix.conf:
../../../Develop/build/qt-embedded-4.7.2/TARGET/mkspecs/common/linux.conf:
../../../Develop/build/qt-embedded-4.7.2/TARGET/mkspecs/common/qws.conf:
../../../Develop/build/qt-embedded-4.7.2/TARGET/mkspecs/qconfig.pri:
../../../Develop/build/qt-embedded-4.7.2/TARGET/mkspecs/features/qt_functions.prf:
../../../Develop/build/qt-embedded-4.7.2/TARGET/mkspecs/features/qt_config.prf:
../../../Develop/build/qt-embedded-4.7.2/TARGET/mkspecs/features/exclusive_builds.prf:
../../../Develop/build/qt-embedded-4.7.2/TARGET/mkspecs/features/default_pre.prf:
SRC/core/core_synchronizer.pri:
../../../Develop/build/qt-embedded-4.7.2/TARGET/mkspecs/features/release.prf:
../../../Develop/build/qt-embedded-4.7.2/TARGET/mkspecs/features/default_post.prf:
../../../Develop/build/qt-embedded-4.7.2/TARGET/mkspecs/features/warn_on.prf:
../../../Develop/build/qt-embedded-4.7.2/TARGET/mkspecs/features/qt.prf:
../../../Develop/build/qt-embedded-4.7.2/TARGET/mkspecs/features/unix/thread.prf:
../../../Develop/build/qt-embedded-4.7.2/TARGET/mkspecs/features/moc.prf:
../../../Develop/build/qt-embedded-4.7.2/TARGET/mkspecs/features/resources.prf:
../../../Develop/build/qt-embedded-4.7.2/TARGET/mkspecs/features/uic.prf:
../../../Develop/build/qt-embedded-4.7.2/TARGET/mkspecs/features/yacc.prf:
../../../Develop/build/qt-embedded-4.7.2/TARGET/mkspecs/features/lex.prf:
/home/jvillasante/hacking_lily/Develop/build/qt-embedded-4.7.2/TARGET/lib/libQtSql.prl:
/home/jvillasante/hacking_lily/Develop/build/qt-embedded-4.7.2/TARGET/lib/libQtCore.prl:
/home/jvillasante/hacking_lily/Develop/build/qt-embedded-4.7.2/TARGET/lib/libQtNetwork.prl:
qmake:  FORCE
	@$(QMAKE) -spec ../../../Develop/build/qt-embedded-4.7.2/TARGET/mkspecs/qws/linux-arm-gnueabi-g++ CONFIG+=release DEFINES+=TARGET -o Makefile SRC/synchronizer/sychronizer.pro

dist: 
	@$(CHK_DIR_EXISTS) SRC/synchronizer/BUILD/synchronizer/obj/synchronizer1.0.0 || $(MKDIR) SRC/synchronizer/BUILD/synchronizer/obj/synchronizer1.0.0 
	$(COPY_FILE) --parents $(SOURCES) $(DIST) SRC/synchronizer/BUILD/synchronizer/obj/synchronizer1.0.0/ && $(COPY_FILE) --parents SRC/synchronizer/headers/synchronizer.h SRC/core/headers/soaphandler.h SRC/core/headers/dbaccess.h SRC/core/headers/idkitwrapper.h SRC/core/headers/utils.h SRC/synchronizer/BUILD/synchronizer/obj/synchronizer1.0.0/ && $(COPY_FILE) --parents SRC/synchronizer/sources/main.cpp SRC/synchronizer/sources/synchronizer.cpp SRC/core/sources/soaphandler.cpp SRC/core/sources/dbaccess.cpp SRC/core/sources/idkitwrapper.cpp SRC/core/sources/utils.cpp SRC/synchronizer/BUILD/synchronizer/obj/synchronizer1.0.0/ && (cd `dirname SRC/synchronizer/BUILD/synchronizer/obj/synchronizer1.0.0` && $(TAR) synchronizer1.0.0.tar synchronizer1.0.0 && $(COMPRESS) synchronizer1.0.0.tar) && $(MOVE) `dirname SRC/synchronizer/BUILD/synchronizer/obj/synchronizer1.0.0`/synchronizer1.0.0.tar.gz . && $(DEL_FILE) -r SRC/synchronizer/BUILD/synchronizer/obj/synchronizer1.0.0


clean:compiler_clean 
	-$(DEL_FILE) $(OBJECTS)
	-$(DEL_FILE) *~ core *.core


####### Sub-libraries

distclean: clean
	-$(DEL_FILE) $(TARGET) 
	-$(DEL_FILE) Makefile


check: first

mocclean: compiler_moc_header_clean compiler_moc_source_clean

mocables: compiler_moc_header_make_all compiler_moc_source_make_all

compiler_moc_header_make_all: BUILD/synchronizer/moc/moc_synchronizer.cpp BUILD/synchronizer/moc/moc_soaphandler.cpp BUILD/synchronizer/moc/moc_dbaccess.cpp
compiler_moc_header_clean:
	-$(DEL_FILE) BUILD/synchronizer/moc/moc_synchronizer.cpp BUILD/synchronizer/moc/moc_soaphandler.cpp BUILD/synchronizer/moc/moc_dbaccess.cpp
BUILD/synchronizer/moc/moc_synchronizer.cpp: SRC/synchronizer/headers/synchronizer.h
	/home/jvillasante/hacking_lily/Develop/build/qt-embedded-4.7.2/TARGET/bin/moc $(DEFINES) $(INCPATH) SRC/synchronizer/headers/synchronizer.h -o BUILD/synchronizer/moc/moc_synchronizer.cpp

BUILD/synchronizer/moc/moc_soaphandler.cpp: SRC/core/headers/soaphandler.h
	/home/jvillasante/hacking_lily/Develop/build/qt-embedded-4.7.2/TARGET/bin/moc $(DEFINES) $(INCPATH) SRC/core/headers/soaphandler.h -o BUILD/synchronizer/moc/moc_soaphandler.cpp

BUILD/synchronizer/moc/moc_dbaccess.cpp: SRC/core/headers/idkitwrapper.h \
		SRC/core/headers/dbaccess.h
	/home/jvillasante/hacking_lily/Develop/build/qt-embedded-4.7.2/TARGET/bin/moc $(DEFINES) $(INCPATH) SRC/core/headers/dbaccess.h -o BUILD/synchronizer/moc/moc_dbaccess.cpp

compiler_rcc_make_all:
compiler_rcc_clean:
compiler_image_collection_make_all: BUILD/synchronizer/ui/qmake_image_collection.cpp
compiler_image_collection_clean:
	-$(DEL_FILE) BUILD/synchronizer/ui/qmake_image_collection.cpp
compiler_moc_source_make_all:
compiler_moc_source_clean:
compiler_uic_make_all:
compiler_uic_clean:
compiler_yacc_decl_make_all:
compiler_yacc_decl_clean:
compiler_yacc_impl_make_all:
compiler_yacc_impl_clean:
compiler_lex_make_all:
compiler_lex_clean:
compiler_clean: compiler_moc_header_clean 

####### Compile

BUILD/synchronizer/obj/main.o: SRC/synchronizer/sources/main.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o BUILD/synchronizer/obj/main.o SRC/synchronizer/sources/main.cpp

BUILD/synchronizer/obj/synchronizer.o: SRC/synchronizer/sources/synchronizer.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o BUILD/synchronizer/obj/synchronizer.o SRC/synchronizer/sources/synchronizer.cpp

BUILD/synchronizer/obj/soaphandler.o: SRC/core/sources/soaphandler.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o BUILD/synchronizer/obj/soaphandler.o SRC/core/sources/soaphandler.cpp

BUILD/synchronizer/obj/dbaccess.o: SRC/core/sources/dbaccess.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o BUILD/synchronizer/obj/dbaccess.o SRC/core/sources/dbaccess.cpp

BUILD/synchronizer/obj/idkitwrapper.o: SRC/core/sources/idkitwrapper.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o BUILD/synchronizer/obj/idkitwrapper.o SRC/core/sources/idkitwrapper.cpp

BUILD/synchronizer/obj/utils.o: SRC/core/sources/utils.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o BUILD/synchronizer/obj/utils.o SRC/core/sources/utils.cpp

BUILD/synchronizer/obj/moc_synchronizer.o: BUILD/synchronizer/moc/moc_synchronizer.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o BUILD/synchronizer/obj/moc_synchronizer.o BUILD/synchronizer/moc/moc_synchronizer.cpp

BUILD/synchronizer/obj/moc_soaphandler.o: BUILD/synchronizer/moc/moc_soaphandler.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o BUILD/synchronizer/obj/moc_soaphandler.o BUILD/synchronizer/moc/moc_soaphandler.cpp

BUILD/synchronizer/obj/moc_dbaccess.o: BUILD/synchronizer/moc/moc_dbaccess.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o BUILD/synchronizer/obj/moc_dbaccess.o BUILD/synchronizer/moc/moc_dbaccess.cpp

####### Install

install:   FORCE

uninstall:   FORCE

FORCE:


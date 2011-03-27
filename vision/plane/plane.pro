# -------------------------------------------------
# Project created by QtCreator 2010-11-03T19:37:14
# -------------------------------------------------

TARGET = plane
CONFIG += console
CONFIG -= app_bundle
TEMPLATE = app
CONFIG += link_pkgconfig

win {

}

unix {
	CONFIG += link_pkgconfig
}

macx {
	PKGCONFIG += /usr/local/lib/pkgconfig/opencv.pc
}

macx {
	QMAKE_CXX = /opt/local/bin/gcc-mp-4.5
	INCLUDEPATH += /usr/local/include
}
linux-g++ {
	QMAKE_CXX = g++-4.5
	LIBS += -lboost_thread-mt
}
QMAKE_CXXFLAGS = -std=c++0x -O3
INCLUDEPATH += ../../ooSockets/src \
    ../common
LIBS += -L/opt/local/lib/gcc45 \
    -L. \
    -lboost_regex \
    -lpthread \
    -lboost_filesystem \
    -lboost_system \
	`pkg-config --libs opencv`

DEFINES += OO_ALL

#macx { 
#    QMAKE_CXX = /opt/local/bin/gcc-mp-4.5
#    LIBS += -L/opt/local/lib/gcc45
#    INCLUDEPATH = /opt/local/include/gcc45/c++ \
#        /usr/include
#}
#linux-g++:QMAKE_CXX = gcc-4.5
#QMAKE_CXXFLAGS = -std=c++0x \
#    -pthread
#INCLUDEPATH += ../../ooSockets/src \
#    ../common
#LIBS += -lboost_regex \
#    -lpthread \
#    -lboost_filesystem \
#    -lboost_system \
#    -lboost_thread \
#    -L/usr/local/lib \
#	-lml \
#	-lcvaux \
#	-lhighgui \
#	-lcv \
#	-lcxcore \
#	-L/opt/local/lib \
#	-lx264
SOURCES += main.cpp \
    MainThread.cpp \
    VideoThread.cpp \
    ../../ooSockets/src/OVersion.cpp \
    ../../ooSockets/src/OUdpSocket.cpp \
    ../../ooSockets/src/OTimer.cpp \
    ../../ooSockets/src/OThread.cpp \
    ../../ooSockets/src/OTcpSocket.cpp \
    ../../ooSockets/src/OTcpServer.cpp \
    ../../ooSockets/src/OString.cpp \
    ../../ooSockets/src/OSocket.cpp \
    ../../ooSockets/src/OSockError.cpp \
    ../../ooSockets/src/OSockAddress.cpp \
    ../../ooSockets/src/OSerialError.cpp \
    ../../ooSockets/src/OSerial.cpp \
    ../../ooSockets/src/OO.cpp \
    ../../ooSockets/src/OProperties.cpp \
    ../../ooSockets/src/OPipeError.cpp \
    ../../ooSockets/src/OPipe.cpp \
    ../../ooSockets/src/ONet.cpp \
    ../../ooSockets/src/OMacAddress.cpp \
    ../../ooSockets/src/OFormLetter.cpp \
    ../../ooSockets/src/OByteArray.cpp \
    ../../ooSockets/src/OAddressInfo.cpp \
    ../common/ImageDatabase.cpp \
    ../../ooSockets/src/OTime.cpp \
    ../../ooSockets/src/OCompatibility.cpp \
    ../../ooSockets/src/ODSMux.cpp \
    ../../ooSockets/src/ODataStream.cpp
HEADERS += MainThread.h \
    VideoThread.h \
    ../../ooSockets/src/OVersion.hpp \
    ../../ooSockets/src/OUdpSocket.hpp \
    ../../ooSockets/src/OTimerBase.hpp \
    ../../ooSockets/src/OTimer.hpp \
    ../../ooSockets/src/OThread.hpp \
    ../../ooSockets/src/OTcpSocket.hpp \
    ../../ooSockets/src/OTcpServer.hpp \
    ../../ooSockets/src/OString.hpp \
    ../../ooSockets/src/OSocket.hpp \
    ../../ooSockets/src/OSockError.hpp \
    ../../ooSockets/src/OSockAddress.hpp \
    ../../ooSockets/src/OSerialError.hpp \
    ../../ooSockets/src/OSerial.hpp \
    ../../ooSockets/src/OO.hpp \
    ../../ooSockets/src/OProperties.hpp \
    ../../ooSockets/src/OPipeError.hpp \
    ../../ooSockets/src/OPipe.hpp \
    ../../ooSockets/src/ONet.hpp \
    ../../ooSockets/src/OMacAddress.hpp \
    ../../ooSockets/src/OList.hpp \
    ../../ooSockets/src/OIODevice.hpp \
    ../../ooSockets/src/OFormLetter.hpp \
    ../../ooSockets/src/OByteArray.hpp \
    ../../ooSockets/src/OAddressInfo.hpp \
    ../common/Defs.h \
    ../common/ImageDatabase.h \
    ../../ooSockets/src/OTime.hpp \
    ../../ooSockets/src/OCompatibility.hpp \
    ../../ooSockets/src/ODSMux.h \
    ../../ooSockets/src/ODataStream.hpp

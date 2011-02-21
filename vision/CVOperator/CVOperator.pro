# -------------------------------------------------
# Project created by QtCreator 2010-06-20T19:31:03
# -------------------------------------------------
QT += network \
    xml
TARGET = CVOperator
TEMPLATE = app
macx {
	QMAKE_CXX = /opt/local/bin/gcc-mp-4.5
	INCLUDEPATH += /usr/local/include
	LIBS += `pkg-config --libs /usr/local/lib/pkgconfig/opencv.pc`
}
linux-g++ {
	QMAKE_CXX = g++-4.5
	LIBS += `pkg-config --libs opencv`
}
QMAKE_CXXFLAGS = -std=c++0x
INCLUDEPATH += ../../ooSockets2.0.2/src \
    ../common
LIBS += -L/opt/local/lib/gcc45 \
    -L. \
    -lboost_regex \
    -lpthread \
    -lboost_filesystem \
    -lboost_system 
SOURCES += main.cpp \
    CVOperator.cpp \
    ../../ooSockets2.0.2/src/OVersion.cpp \
    ../../ooSockets2.0.2/src/OUdpSocket.cpp \
    ../../ooSockets2.0.2/src/OTimer.cpp \
    ../../ooSockets2.0.2/src/OTime.cpp \
    ../../ooSockets2.0.2/src/OThread.cpp \
    ../../ooSockets2.0.2/src/OTcpSocket.cpp \
    ../../ooSockets2.0.2/src/OTcpServer.cpp \
    ../../ooSockets2.0.2/src/OString.cpp \
    ../../ooSockets2.0.2/src/OSocket.cpp \
    ../../ooSockets2.0.2/src/OSockError.cpp \
    ../../ooSockets2.0.2/src/OSockAddress.cpp \
    ../../ooSockets2.0.2/src/OSerialError.cpp \
    ../../ooSockets2.0.2/src/OSerial.cpp \
    ../../ooSockets2.0.2/src/OO.cpp \
    ../../ooSockets2.0.2/src/OProperties.cpp \
    ../../ooSockets2.0.2/src/OPipeError.cpp \
    ../../ooSockets2.0.2/src/OPipe.cpp \
    ../../ooSockets2.0.2/src/ONet.cpp \
    ../../ooSockets2.0.2/src/OMacAddress.cpp \
    ../../ooSockets2.0.2/src/OFormLetter.cpp \
    ../../ooSockets2.0.2/src/OByteArray.cpp \
    ../../ooSockets2.0.2/src/OAddressInfo.cpp \
    ../common/ImageDatabase.cpp
HEADERS += CVOperator.h \
    ../../ooSockets2.0.2/src/OVersion.hpp \
    ../../ooSockets2.0.2/src/OUdpSocket.hpp \
    ../../ooSockets2.0.2/src/OTimerBase.hpp \
    ../../ooSockets2.0.2/src/OTimer.hpp \
    ../../ooSockets2.0.2/src/OTime.hpp \
    ../../ooSockets2.0.2/src/OThread.hpp \
    ../../ooSockets2.0.2/src/OTcpSocket.hpp \
    ../../ooSockets2.0.2/src/OTcpServer.hpp \
    ../../ooSockets2.0.2/src/OString.hpp \
    ../../ooSockets2.0.2/src/OSocket.hpp \
    ../../ooSockets2.0.2/src/OSockError.hpp \
    ../../ooSockets2.0.2/src/OSockAddress.hpp \
    ../../ooSockets2.0.2/src/OSerialError.hpp \
    ../../ooSockets2.0.2/src/OSerial.hpp \
    ../../ooSockets2.0.2/src/OO.hpp \
    ../../ooSockets2.0.2/src/OProperties.hpp \
    ../../ooSockets2.0.2/src/OPipeError.hpp \
    ../../ooSockets2.0.2/src/OPipe.hpp \
    ../../ooSockets2.0.2/src/ONet.hpp \
    ../../ooSockets2.0.2/src/OMacAddress.hpp \
    ../../ooSockets2.0.2/src/OList.hpp \
    ../../ooSockets2.0.2/src/OIODevice.hpp \
    ../../ooSockets2.0.2/src/OFormLetter.hpp \
    ../../ooSockets2.0.2/src/OByteArray.hpp \
    ../../ooSockets2.0.2/src/OAddressInfo.hpp \
    ../common/Defs.h \
    ../common/ImageDatabase.h
FORMS += CVOperator.ui

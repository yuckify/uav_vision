# -------------------------------------------------
# Project created by QtCreator 2010-06-20T19:31:03
# -------------------------------------------------
QT += network \
    xml \
	sql
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
INCLUDEPATH += ../../ooSockets/src \
    ../common
LIBS += -L/opt/local/lib/gcc45 \
    -L. \
    -lboost_regex \
    -lpthread \
    -lboost_filesystem \
    -lboost_system 
SOURCES += main.cpp \
    CVOperator.cpp \
    ../../ooSockets/src/OVersion.cpp \
    ../../ooSockets/src/OUdpSocket.cpp \
    ../../ooSockets/src/OTimer.cpp \
    ../../ooSockets/src/OTime.cpp \
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
    LogWidget.cpp
HEADERS += CVOperator.h \
    ../../ooSockets/src/OVersion.hpp \
    ../../ooSockets/src/OUdpSocket.hpp \
    ../../ooSockets/src/OTimerBase.hpp \
    ../../ooSockets/src/OTimer.hpp \
    ../../ooSockets/src/OTime.hpp \
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
    LogWidget.h
FORMS += CVOperator.ui

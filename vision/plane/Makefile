SOURCES=../../ooSockets/src/OAddressInfo.cpp \
../../ooSockets/src/OByteArray.cpp \
../../ooSockets/src/OCompatibility.cpp \
../../ooSockets/src/ODataStream.cpp \
../../ooSockets/src/ODSMux.cpp \
../../ooSockets/src/OFormLetter.cpp \
../../ooSockets/src/OMacAddress.cpp \
../../ooSockets/src/ONet.cpp \
../../ooSockets/src/OO.cpp \
../../ooSockets/src/OPipe.cpp \
../../ooSockets/src/OPipeError.cpp \
../../ooSockets/src/OProperties.cpp \
../../ooSockets/src/OSerial.cpp \
../../ooSockets/src/OSerialError.cpp \
../../ooSockets/src/OSockAddress.cpp \
../../ooSockets/src/OSockError.cpp \
../../ooSockets/src/OSocket.cpp \
../../ooSockets/src/OString.cpp \
../../ooSockets/src/OTcpServer.cpp \
../../ooSockets/src/OTcpSocket.cpp \
../../ooSockets/src/OThread.cpp \
../../ooSockets/src/OTime.cpp \
../../ooSockets/src/OTimer.cpp \
../../ooSockets/src/OUdpSocket.cpp \
../../ooSockets/src/OVersion.cpp \
main.cpp \
MainThread.cpp \
VideoThread.cpp \
../common/ImageDatabase.cpp
CC=g++
CFLAGS=-c -std=c++0x -Wall -DOO_OPENCV -I../../ooSockets/src -I. -I../common `pkg-config --cflags opencv`
LDFLAGS=`pkg-config --libs opencv` -lboost_regex -lpthread -lboost_filesystem -lboost_system
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=plane
all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
#$(SOURCES):
#	$(CC) $(CFLAGS) $@ -o $(@:.cpp=.o)
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm $(EXECUTABLE) $(OBJECTS)

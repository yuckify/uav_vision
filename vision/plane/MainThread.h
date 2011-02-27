#ifndef MAINTHREAD_H
#define MAINTHREAD_H

#include<deque>
//#include<thread>
//#include<mutex>
#include<sys/types.h>
#include<memory>

#include<boost/thread/mutex.hpp>
#include<boost/filesystem.hpp>

namespace bst = boost;


#include<OThread.hpp>
#include<OUdpSocket.hpp>
#include<OTcpSocket.hpp>
#include<OByteArray.hpp>
#include<OTcpServer.hpp>
#include<OList.hpp>
#include<OPipe.hpp>
#include<OSerial.hpp>
#include<OTimer.hpp>
#include<OSocket.hpp>
#include<ONet.hpp>


#include"VideoThread.h"
#include"Defs.h"
#include"ImageDatabase.h"

#define DEBUG 1

#define DBPATH		"db"
#define DBFILENAME DBPATH"/imagedb.txt"

namespace bfs = boost::filesystem;

using namespace std;

void DebugMsg(OString str);

class MainThread : public OThread {
public:
	MainThread();
	
	
	
private:
	enum HostType {
		Ground,
		Video
	};
	
	//this is the multicast listener for when the
	//ground station trys to discover the plane
	OUdpSocket* multping;
	void multError(OSockError e);
	void multTimeout();
	OTimer multTimer;
	
	//this socket is for listening for incomming connections from 
	//the ground, when a connection is opened the socket is placed
	//in the 'unid' list. the connecting host then has to send an
	//'Identify' packet so this host knows how to interact it.
	OTcpServer* serv;
	void incommingConnection(OO::HANDLE fd);
	
	//this socket is for receiving data from the video thread
	VideoThread* vthread;
	OPipe* video;
	void videoRead();
	
	//this socket is for communicating with the vision ground station
	OTcpSocket* ground;
	void groundDisconnected();
	void groundReadyRead();
	void groundReadyWrite();
	
	//this serial thing is used to initialliy read from the serial
	//ports, the data is then parsed so which device we are talking
	//to may be determined
	OSerial initserial;
	OTimer initsertimer;
	OString tmpserbuf;
	void initSerialRead();
	
	//this is the serial interface for talking to the camera
	//controller
	OSerial camcontrol;
	OString cambuff;
	void camControlRead();
	
	//this is the serial interface for receiving the data from the
	//autopilot which will be forwarded to the ground station
	OSerial autopilot;
	OString serialbuf;
	long long packnum;
	void autoPilotRead();
	
	//these are the queues where the data is push into from
	//the serial port and the video thread
	bst::mutex videolock;
	deque<OByteArray> videopacks;
	
	//this is the serial interface to send camera commands to the 
	//controlling arduino
	unique_ptr<OSerial> arduino;
	
	//this is the database where the info for each image is stored
	ImageDatabase db;
	
	PlaneInfo info;
};


#endif // MAINTHREAD_H

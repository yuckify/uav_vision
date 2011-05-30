#ifndef MAINTHREAD_H
#define MAINTHREAD_H

#include<sys/types.h>
#include<memory>

#include<boost/thread/mutex.hpp>
#include<boost/filesystem.hpp>
#include<boost/scoped_ptr.hpp>

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
#include<ODataStream.hpp>

#include"VideoThread.h"
#include"Defs.h"
#include"ImageDatabase.h"

#define DEBUG 1

#define DBPATH		"db"
#define DBFILENAME DBPATH"/imagedb.txt"
#define IMAGEDIR	DBPATH"/images"

namespace bfs = boost::filesystem;
using namespace std::placeholders;

using namespace std::placeholders;
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
	
	//when this function is called, the image db is send to the ground
	void writeImageDb();
	
	//this socket is for receiving data from the video thread
	VideoThread* vthread;
	
	//this socket is for communicating with the vision ground station
//	OTcpSocket* ground;
	void groundDisconnected();
	
	ODataStream<> gc;
	
	//callback methods for networking
	void imageDetails(OByteArray data);
	void compressionMethod(OByteArray data);
	void cameraZoomIn(OByteArray data);
	void cameraZoomOut(OByteArray data);
	void cameraCapture(OByteArray data);
	void cameraPower(OByteArray data);
	void cameraDownload(OByteArray data);
	
	//this structure and these functions are for packet switching
	OList<void (*)(OByteArray&)> switcher;
	
	//this timer executes after about 1/2 second when the camera
	//mounts as a flash drive
	OTimer cameratimer;
	void readCameraFiles();
	bfs::path find_files(bfs::path p);
	
	
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
	bool add_capture;
	void camControlRead();
	bool validCharacters(OString& str);
	
	//this is the serial interface for receiving the data from the
	//autopilot which will be forwarded to the ground station
	OSerial autopilot;
	OString serialbuf;
	long long packnum;
	void autoPilotRead();
	
	//this is the serial interface to send camera commands to the 
	//controlling arduino
	boost::scoped_ptr<OSerial> arduino;
	
	//this is the database where the info for each image is stored
	ImageDatabase db;
	
	PlaneInfo info;
};


#endif // MAINTHREAD_H

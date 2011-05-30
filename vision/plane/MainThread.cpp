#include"MainThread.h"

void DebugMsg(OString str) {
#ifdef DEBUG
	cout<<str <<endl;
#endif
}

MainThread::MainThread() :
	multTimer(this), gc(this), cameratimer(this), 
	initserial(this), initsertimer(this) {
	//setup the database files
	if(bfs::is_directory(DBPATH)) {
		//if the directory exists then this is not a cold start
		//reload the pre-existing database files
		db.load(DBFILENAME);
		
	} else {
		//this is a cold start, create the necessary files
		bfs::create_directory(DBPATH);
		bfs::create_directory(IMAGEDIR);
		
	}
	
	OPortList ports = OSerial::portList().filterByType(OO::UsbPort);
	if(ports.size() == 0) {
		cerr<<"*****No available serial ports.*****" <<endl;
//		::exit(0);
	} else {
		cout<<"name: " <<ports[0].hwName() <<endl;
		initserial.readFunc(bind(&MainThread::initSerialRead, this));
		initserial.setSpeed(OO::O115200);
		initserial.open(ports[0]);
	}
	
	add_capture = false;
	
	packnum = 0;
	//at this point we don't actually know what the autopilot and camera control
	//serial ports are 
	//first we have to get some output from the port and then determine if 
	//which one we are currently looking at
	//the we connect to another port and rinse and repeat
	
	//setup the camera timer
	cameratimer.callback(bind(&MainThread::readCameraFiles, this));
	
	//setup the socket to send multicast packets to
	//discovert the groundstation
	multping = new OUdpSocket(this);
	multping->errorFunc(bind(&MainThread::multError, this, std::placeholders::_1));
	multping->sendMulticast(25000, "225.0.0.37");
	
	serv = new OTcpServer(this);
	serv->incommingFunc(bind(&MainThread::incommingConnection, this, std::placeholders::_1));
	serv->listen(25001);
	
	//**************************************************
	//
	//**************************************************
	gc.setSendHandler(VideoFrame, PriorityHigh);
	gc.setSendHandler(ImageDetails, PriorityMed);
	gc.setDisconnectSig(bind(&MainThread::groundDisconnected, this));
	//setup the receive handlers
	gc.setRecvHandler(CameraZoomIn, boost::bind(&MainThread::cameraZoomIn, 
										 this, std::placeholders::_1));
	gc.setRecvHandler(CameraZoomOut, boost::bind(&MainThread::cameraZoomOut, 
										 this, std::placeholders::_1));
	gc.setRecvHandler(CameraCapture, boost::bind(&MainThread::cameraCapture, 
										 this, std::placeholders::_1));
	gc.setRecvHandler(CameraPower, boost::bind(&MainThread::cameraPower, 
										 this, std::placeholders::_1));
	gc.setRecvHandler(CameraDownload, boost::bind(&MainThread::cameraDownload, 
										 this, std::placeholders::_1));
	gc.setRecvHandler(CompressionMethod, boost::bind(&MainThread::compressionMethod, 
										 this, std::placeholders::_1));
	gc.setRecvHandler(ImageDetails, boost::bind(&MainThread::imageDetails, 
										 this, std::placeholders::_1));
	
	/*
	
	gc.setRecvHandler(CameraZoomIn, [](OByteArray data)->void{
		cout<<"CameraZoomIn" <<endl;
	});
	
	gc.setRecvHandler(CameraZoomOut, [](OByteArray data)->void{
		cout<<"CameraZoomOut" <<endl;
	});
	
	gc.setRecvHandler(CameraCapture, [](OByteArray data)->void{
		cout<<"CameraCapture" <<endl;
	});
	
	gc.setRecvHandler(CameraPower, [](OByteArray data)->void{
		cout<<"CameraPower" <<endl;
	});
	
	gc.setRecvHandler(CameraDownload, [](OByteArray data)->void{
		cout<<"CameraDownload" <<endl;
	});
	
	gc.setRecvHandler(CompressionMethod, [&vthread](OByteArray data)->void{
		cout<<"CompressionMethod" <<endl;
		OString comp;
		data>>comp;
		vthread->setCompression(comp);
	});
	
	gc.setRecvHandler(ImageDetails, [this](OByteArray data)->void{
		cout<<"ImageDetails" <<endl;
		this->writeImageDb();
	});
	*/
	//**************************************************
	//
	//**************************************************
	
	//initialize the video thread to handle grabbing the video frames
	vthread = new VideoThread(&gc, info);
	vthread->start();
	
	//setup the timeout function so the multicast socket
	//emits a packet every one second or so
	multTimer.callback(bind(&MainThread::multTimeout, this));
	multTimer.start(100, OO::Repeat);
	
	
	//setup the serial connection to the arduino, so we can send it commands
	//to control the camera
	arduino.reset(new OSerial());
	
}

void MainThread::compressionMethod(OByteArray data) {
	cout<<"CompressionMethod" <<endl;
	OString comp;
	data>>comp;
	vthread->setCompression(comp);
}

void MainThread::cameraZoomIn(OByteArray data) {
	cout<<"CameraZoomIn" <<endl;
}

void MainThread::cameraZoomOut(OByteArray data) {
	cout<<"CameraZoomOut" <<endl;
}

void MainThread::cameraCapture(OByteArray data) {
	cout<<"CameraCapture" <<endl;
}
void MainThread::cameraPower(OByteArray data) {
	cout<<"CameraPower" <<endl;
}

void MainThread::cameraDownload(OByteArray data) {
	cout<<"CameraDownload" <<endl;
}

void MainThread::imageDetails(OByteArray data) {
	cout<<"ImageDetails" <<endl;
	this->writeImageDb();
}

void MainThread::multTimeout() {
	if(!gc.socket().connected()) {
		OByteArray pack;
		pack<<123;
		multping->write(pack);
	}
}

void MainThread::multError(OSockError e) {
	cout<<e.code() <<" " <<e.string() <<endl;
}

void MainThread::incommingConnection(OO::HANDLE fd) {
	if(!gc.socket().connected()) {
		cout<<"New Ground Socket" <<endl;
		multTimer.stop();
		gc.socket().setHandle(fd);
	}
}

void MainThread::writeImageDb() {
	if(gc.socket().connected()) {
		OByteArray pack;
		pack<<db;
//		gc.write(ImageDetails, db);
	}
}

void MainThread::groundDisconnected() {
	cout<<"Ground Disconnected" <<endl;
	
	multTimer.parent(this);
	multTimer.callback(bind(&MainThread::multTimeout, this));
	multTimer.start(100, OO::Repeat);
}

void MainThread::readCameraFiles() {
	bfs::path sdir("/media");
	bfs::path fdir = find_files(sdir);
	if(bfs::exists(fdir)) {
		bfs::path destdir(IMAGEDIR);
		
		int index = 0;
		bfs::directory_iterator end;
		for(bfs::directory_iterator i(fdir); i!=end; i++, index++) {
#ifdef __apple__
			db[index].i_name = i->path().filename().string();
#else
			//works on linux not mac os
			db[index].i_name = i->filename();
#endif
			if(!db[index].i_downloaded) {
				//setup the destination path
				bfs::path impath(destdir);
#ifdef __apple__
				impath /= i->path().filename().string();
#else
				//works on linux not mac os
				impath /= i->filename();
#endif
				
				//copy the file off the camera
				bfs::copy_file(*i, impath);
				
				db[index].i_downloaded = true;
			}
		}
	}
	
	db.save(DBFILENAME);
	
	this->writeImageDb();
	
	/*
	camcontrol.close();
	camcontrol.open(OO::O115200, camcontrol.port());
	*/
	
	OByteArray msg;
	msg<<(char)UsbDisable;
	
	camcontrol.write(msg);
}

bfs::path MainThread::find_files(bfs::path p) {
	bfs::directory_iterator end;
	for(bfs::directory_iterator i(p); i!=end; i++) {
		if(bfs::is_directory(*i)) {
			return find_files(*i);
		} else if(i->path().string().find(".JPG")) {
			//old, work on linux not mac os
//		} else if(i->string().find(".JPG")) {
			return p;
		}
	}
	return bfs::path();
}

void MainThread::initSerialRead() {
	
	OByteArray data = initserial.readAll();
	
	for(unsigned i=0; i<data.size(); i++) {
		if(data.data()[i])
			tmpserbuf.push_back(data.data()[i]);
	}
	
	if(tmpserbuf.contains("camera_controller\r\n")) {
		cout<<"Found Camera Controller" <<endl;
		OByteArray msg;
		msg<<'r';
		initserial.write(msg);
		
		camcontrol = initserial;
		camcontrol.readFunc(bind(&MainThread::camControlRead, this));
		camcontrol.parent(this);
	}
	
}

void MainThread::camControlRead() {
	do {
		OByteArray data = camcontrol.read(1);
		char xfer = 0;
		if(data.size() < 1) break;
		data>>xfer;
		cambuff.push_back(xfer);
		
		if(xfer == '\n') {
			if(validCharacters(cambuff)) {
				
				int state = 0;
				if(cambuff == "$camera_controller\r\n") {
					cambuff.clear();
				} else if(cambuff == "$Unknown\r\n") {
					state = Camera::Unknown;
					cambuff.clear();
				} else if(cambuff == "$Ready\r\n") {
					add_capture = false;
					state = Camera::Ready;
					cambuff.clear();
				} else if(cambuff == "$CameraSleeping\r\n") {
					state = Camera::Sleeping;
					cambuff.clear();
				} else if(cambuff == "$Zooming\r\n") {
					state = Camera::Zooming;
					cambuff.clear();
				} else if(cambuff == "$Capturing\r\n") {
					state = Camera::Capturing;
					cambuff.clear();
				} else if(cambuff == "$DoneCapturing\r\n") {
					if(!add_capture) {
						//add the newly capture image to the db
						db.add(ImageInfo("", info.yaw, info.pitch, info.roll, 
										 info.x, info.y, info.alt, false, false));
						
						//save the database to the hard drive so we have a backup copy
						//of it incase this program crashes, worst case scenario
						db.save(DBFILENAME);
						
						this->writeImageDb();
						
						add_capture = true;
					}
					state = Camera::Ready;
					cambuff.clear();
				} else if(cambuff == "$Downloading\r\n") {
					state = Camera::Downloading;
					cambuff.clear();
				} else {
					cambuff.clear();
				}
				
				if(gc.socket().connected() && state) {
					OByteArray pack;
					pack<<state;
//					gc.write(CameraStatus, pack);
				}
			}
		}
	} while(camcontrol.available());
	
}

bool MainThread::validCharacters(OString &str) {
	int length = str.length();
	for(int i=0; i<length; i++) {
		if(str[i] != '$' && (str[i] < 'a' || str[i] > 'z') &&
		   (str[i] < 'A' || str[i] > 'Z') && str[i] != '\r' &&
		   str[i] != '\n' && str[i] != '_') {
			return false;
		}
	}
	return true;
}

void MainThread::autoPilotRead() {
	packnum++;
	OByteArray data = autopilot.readAll();
	
	for(unsigned i=0; i<data.size(); i++) {
		if(!data.data()[i]) return;
	}
	
	//read the data from the raw binary data container and push it in
	//the string packet buffer
	for(unsigned i=0; i<data.size(); i++) {
		char ch = data.data()[i];
		serialbuf.push_back(ch);
	}
	
	//check if we have enough data in the buffer to pop off a packet
	if(serialbuf.contains("\r\n")) {
		OStringList packs = serialbuf.split("\r\n");
		if(packs.size() > 1) {
			for(unsigned i=0; i<packs.size()-1; i++) {
				
				OStringList args = packs[i].substring(2).split("|");
				bool ok0, ok1, ok2, ok3, ok4, ok5;
				uint32_t yaw, pitch, roll, x, y, alt;
				
				//make sure the packet contains the "++" start sequence
				//just to be sure we did not get a warped half packet
				if(packs[i].startsWith("++")) {
					//get the roll from the autopilot
					roll = args[3].toInt(&ok0);
					if(!ok0) roll = 0;
					
					//get the pitch from the autopilot
					pitch = args[4].toInt(&ok1);
					if(!ok1) pitch = 0;
					
					yaw = args[6].toInt(&ok5);
					if(!ok5) yaw = 0;
					
					info.lock();
					info.roll = roll;
					info.pitch = pitch;
					info.yaw = yaw;
					info.unlock();
				} else if(packs[i].startsWith("!!")) {
					//get the latitude from the autopilot
					x = args[0].toInt(&ok2);
					if(!ok2) x = 0;
					
					//get the longitude from the autopilot
					y = args[1].toInt(&ok3);
					if(!ok3) y = 0;
					
					//get the altitude above the ground
					alt = args[5].toInt(&ok4);
					if(!ok4) alt = 0;
					
					info.lock();
					info.x = x;
					info.y = y;
					info.alt = alt;
					info.unlock();
				}
			}
			//just in case the last split in the list of split packets is
			//a half packet set the buffer to the this half packets
			//and append more data to the end of this
			if(packs.size())
				serialbuf = packs[packs.size()-1];
		}
	}
}

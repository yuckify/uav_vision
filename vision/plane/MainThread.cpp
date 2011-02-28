#include"MainThread.h"

void DebugMsg(OString str) {
#ifdef DEBUG
	cout<<str <<endl;
#endif
}

MainThread::MainThread() :
	initserial(this), initsertimer(this), multTimer(this) {
	//setup the database files
	if(bfs::is_directory(DBPATH)) {
		//if the directory exists then this is not a cold start
		//reload the pre-existing database files
		db.load(DBFILENAME);
		
	} else {
		//this is a cold start, create the necessary files
		bfs::create_directory(DBPATH);
		
	}
	
	//setup the serial initializer
//	initsertimer.callback(bind(&MainThread::initSerialRead, this));
//	initsertimer.start(1000, OO::Once);
	
	OPortList ports = OSerial::portList().filterByType(OO::UsbPort);
	if(ports.size() == 0) {
		cerr<<"*****No available serial ports.*****" <<endl;
		::exit(0);
	}
	cout<<"name: " <<ports[0].hwName() <<endl;
	initserial.readyReadFunc(bind(&MainThread::initSerialRead, this));
	initserial.open(OO::O115200, ports[0], OO::DefaultOpts | OO::NonBlock);
	
	packnum = 0;
	//at this point we don't actually know what the autopilot and camera control
	//serial ports are 
	//first we have to get some output from the port and then determine if 
	//which one we are currently looking at
	//the we connect to another port and rinse and repeat
	
	
	
	//setup the socket to send multicast packets to
	//discovert the groundstation
	multping = new OUdpSocket(this);
	multping->errorFunc(bind(&MainThread::multError, this, _1));
	multping->sendMulticast(25000, "225.0.0.37");
	
	//setup the server socket
	serv = new OTcpServer(this);
	serv->incommingFunc(bind(&MainThread::incommingConnection, this, _1));
	serv->listen(25001);
	
	//initialize the ground socket pointer
	ground = 0;
	
	//initialize the pipe to get messages from the video thread
	video = new OPipe(this);
	video->readFunc(bind(&MainThread::videoRead, this));
	
	//initialize the video thread to handle grabbing the video frames
	vthread = new VideoThread(videolock, videopacks, *video, info);
	vthread->start();
	
	//setup the timeout function so the multicast socket
	//emits a packet every one second or so
	multTimer.callback(bind(&MainThread::multTimeout, this));
	multTimer.start(100, OO::Repeat);
	
	//setup the serial connection to the arduino, so we can send it commands
	//to control the camera
	arduino.reset(new OSerial());
}

void MainThread::multTimeout() {
	if(!ground) {
//		DebugMsg("Emitted Multicast Discover Packet");
		OByteArray pack;
		pack<<123;
		multping->write(pack);
	}
}

void MainThread::multError(OSockError e) {
	cout<<e.code() <<" " <<e.string() <<endl;
}

void MainThread::incommingConnection(OO::HANDLE fd) {
	if(!ground) {
		cout<<"New Ground Socket" <<endl;
		
		ground = new OTcpSocket(this);
		ground->disconnectFunc(bind(&MainThread::groundDisconnected, this));
		ground->readyReadFunc(bind(&MainThread::groundReadyRead, this));
		ground->readyWriteFunc(bind(&MainThread::groundReadyWrite, this));
		ground->fileDescriptor(fd);
		
		if(db.size()) {
			this->writeImageDb();
		}
		
		if(videopacks.size()) ground->enableReadyWrite();
		
		multTimer.stop();
	}
}

void MainThread::writeImageDb() {
	OByteArray data;
	
	PacketLength dlen = 0;
	PacketType dtype = ImageDetails;
	
	//serialize all the data into the binary container
	data<<dlen <<dtype <<db;
	
	//go back to the beginning of the binary data container
	data.seek(0, OO::beg);
	
	//insert the actual length of the packet
	dlen = data.size() - sizeof(dlen);
	data<<dlen;
	
	//write the data to the socket
	ground->write(data);
}

void MainThread::videoRead() {
	OByteArray data = video->readAll();
	
	if(ground) {
		if(data.size() > 1) {
			//more then one byte was written to the socket, this means that some useful
			//information was written to the pipe, forward this information to the
			//ground
			OByteArray pack;
			
			PacketType type = ErrorMsg;
			PacketLength length;
			
			pack<<length <<type <<data;
			length = pack.size() - sizeof(PacketLength);
			pack.seek(0);
			pack<<length;
			
			ground->write(pack);
		} else {
			//only one byte was written to the pipe, this is the signal to enable 
			//the ready read callback with the ground socket
			ground->enableReadyWrite();
		}
	}
}

void MainThread::groundDisconnected() {
	if(ground) {
		cout<<"Ground Disconnected" <<endl;
		
		delete ground;
		ground = NULL;
		
		videopacks.clear();
		
		multTimer.parent(this);
		multTimer.callback(bind(&MainThread::multTimeout, this));
		multTimer.start(1000, OO::Repeat);
	}
}

void MainThread::groundReadyRead() {
	OByteArray head = ground->read(sizeof(PacketLength));
	
	PacketLength length;
	head>>length;
	
	while(length < ground->available()) {
		usleep(100);
	}
	
	OByteArray pack = ground->read(length);
	
	PacketType type;
	pack>>type;
	
	switch(type) {
	case ImageDetails: {
			this->writeImageDb();
			break;
		}
		case CompressionMethod: {
			OString tmp;
			pack>>tmp;
			vthread->setCompression(tmp);
			
			break;
		}
	case CameraZoomIn: {
			
			break;
		}
	case CameraZoomOut: {
			
			break;
		}
	case CameraCapture: {
			//send the signal to the arduino to capture an image
			//TODO
			
			OByteArray msg;
			msg<<(char)Capture;
			
			camcontrol.write(msg);
			
			//add the newly capture image to the db
			db.add(ImageInfo("", info.yaw, info.pitch, info.roll, 
							 info.x, info.y, info.alt, false, false));
			
			//save the database to the hard drive so we have a backup copy
			//of it incase this program crashes, worst case scenario
			db.save(DBFILENAME);
			
			break;
		}
	case CameraPower: {
			cout<<"Camera Power" <<endl;
			OByteArray msg;
			msg<<(char)Power;
			
			camcontrol.write(msg);
			
			break;
		}
	case CameraDownload: {
			cout<<"corrupting" <<endl;
			
			OByteArray data;
			data<<65325 <<45892 <<32457 <<45789;
			
			ground->write(data);
			
			break;
		}
	}
}

void MainThread::groundReadyWrite() {
	if(videopacks.size()) {
		OByteArray pack = videopacks.front();
		videopacks.pop_front();
		ground->write(pack);
	}
}

void MainThread::initSerialRead() {
	
	OByteArray data = initserial.readAll();
	
	for(int i=0; i<data.size(); i++) {
		if(data.data()[i])
			tmpserbuf.push_back(data.data()[i]);
	}
	
	if(tmpserbuf.contains("camera_controller\r\n")) {
		cout<<"Found Camera Controller" <<endl;
		OByteArray msg;
		msg<<'r';
		initserial.write(msg);
		
		camcontrol = initserial;
		camcontrol.readyReadFunc(bind(&MainThread::camControlRead, this));
	}
	
}

void MainThread::camControlRead() {
	
	
}

void MainThread::autoPilotRead() {
	packnum++;
	OByteArray data = autopilot.readAll();
	
	for(int i=0; i<data.size(); i++) {
		if(!data.data()[i]) return;
	}
	
	//read the data from the raw binary data container and push it in
	//the string packet buffer
	for(int i=0; i<data.size(); i++) {
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

#include"MainThread.h"

void DebugMsg(OString str) {
#ifdef DEBUG
	cout<<str <<endl;
#endif
}

MainThread::MainThread() :
	initserial(this), initsertimer(this), multTimer(this), 
	cameratimer(this), gc(this) {
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
		::exit(0);
	}
	cout<<"name: " <<ports[0].hwName() <<endl;
	initserial.readyReadFunc(bind(&MainThread::initSerialRead, this));
	initserial.open(OO::O115200, ports[0], OO::DefaultOpts | OO::NonBlock);
	
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
	multping->errorFunc(bind(&MainThread::multError, this, _1));
	multping->sendMulticast(25000, "225.0.0.37");
	
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
	
	//**************************************************
	//
	//**************************************************
	
	gc.setRecvHandler(ImageDetails, [](OByteArray data)->void{
		cout<<"ImageDetails" <<endl;
	});
	
	gc.setRecvHandler(CompressionMethod, [](OByteArray data)->void{
		cout<<"CompessionMethod" <<endl;
	});
	
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
	
	
	
	//**************************************************
	//
	//**************************************************
}

void MainThread::multTimeout() {
	if(!ground) {
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
		multTimer.stop();
		gc.setFileDescriptor(fd);
		
		/*
		ground = new OTcpSocket(this);
		ground->disconnectFunc(bind(&MainThread::groundDisconnected, this));
		ground->readyReadFunc(bind(&MainThread::groundReadyRead, this));
		ground->readyWriteFunc(bind(&MainThread::groundReadyWrite, this));
		ground->errorFunc(bind(&MainThread::groundError, this));
		ground->setFileDescriptor(fd);
		
		if(db.size()) {
			this->writeImageDb();
		}
		*/
		
		if(videopacks.size()) ground->enableReadyWrite();
	}
}

void MainThread::writeImageDb() {
	if(ground) {
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
		multTimer.start(100, OO::Repeat);
	}
}

void MainThread::groundReadyRead() {
	do {
		OByteArray head = ground->read(sizeof(PacketLength));
		
		PacketLength length;
		head>>length;
		
		while(length > ground->available()) {
			usleep(100);
		}
		
		
		OByteArray pack = ground->read(length);
		
		handlePacket(pack);
	} while(ground->available() > 1000);
	
}

void MainThread::groundReadyWrite() {
	if(videopacks.size()) {
		OByteArray pack = videopacks.front();
		ground->write(pack);
		videopacks.pop_front();
	}
}

void MainThread::groundError() {
	cout<<"Error: " <<ground->error() <<" " <<ground->strerror() <<endl;
}

void MainThread::handlePacket(OByteArray &pack) {
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
			cout<<"Zoom In" <<endl;
			int zlen = 1;
			pack>>zlen;
			OString zoomstr;
			zoomstr<<zlen;
			
			OByteArray msg;
			msg<<zoomstr <<(char)ZoomIn;
			
			camcontrol.write(msg);
			break;
		}
	case CameraZoomOut: {
			cout<<"Zoom Out" <<endl;
			int zlen = 1;
			pack>>zlen;
			OString zoomstr;
			zoomstr<<zlen;
			
			OByteArray msg;
			msg<<zoomstr <<(char)ZoomOut;
			
			camcontrol.write(msg);
			break;
		}
	case CameraCapture: {
			cout<<"Capture" <<endl;
			OByteArray msg;
			msg<<(char)Capture;
			
			camcontrol.write(msg);
			break;
		}
	case CameraPower: {
			OByteArray msg;
			msg<<(char)Power;
			
			camcontrol.write(msg);
			
			break;
		}
	case CameraDownload: {
			cout<<"Download" <<endl;
			OByteArray msg;
			msg<<(char)UsbEnable;
			
			camcontrol.write(msg);
			
			cameratimer.start(5000);
			
			break;
		}
	}
}

void MainThread::readCameraFiles() {
	bfs::path sdir("/media");
	bfs::path fdir = find_files(sdir);
	if(bfs::exists(fdir)) {
		bfs::path destdir(IMAGEDIR);
		
		int index = 0;
		bfs::directory_iterator end;
		for(bfs::directory_iterator i(fdir); i!=end; i++, index++) {
			db[index].i_name = i->filename();
			if(!db[index].i_downloaded) {
				//setup the destination path
				bfs::path impath(destdir);
				impath /= i->filename();
				
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
		} else if(i->string().find(".JPG")) {
			return p;
		}
	}
	return bfs::path();
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
				
				if(ground && state) {
					OByteArray pack;
					PacketType type = CameraStatus;
					PacketLength length = 0;
					
					pack<<length <<type <<state;
					
					pack.seek(0);
					length = pack.size() - sizeof(PacketLength);
					pack<<length;
					
					ground->write(pack);
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

#include "CVOperator.h"
#include "ui_CVOperator.h"

CVOperator::CVOperator(QWidget *parent) : 
	QMainWindow(parent), ui(new Ui::CVOperator),
	gc(this) {
	ui->setupUi(this);
	skip = -1;
	
	this->aligned = true;
	this->last_search = 0;
	this->source_socket = true;
	
	//setup the packet switcher
	for(int i=0; i<100; i++) switcher.push_back(NULL);
//	switcher[VideoFrameHeader] = bind(&CVOperator::VideoFrameHeaderSwitch, this, _1);
//	switcher[VideoFrameSegment] = bind(&CVOperator::VideoFrameSegmentSwitch, this, _1);
	switcher[ImageDetails] = bind(&CVOperator::ImageDetailsSwitch, this, _1);
	switcher[ErrorMsg] = bind(&CVOperator::ErrorMsgSwitch, this, _1);
	switcher[CameraStatus] = bind(&CVOperator::CameraStatusSwitch, this, _1);
	
	//setup the image update timer
	QTimer* iutimer = new QTimer(this);
	connect(iutimer, SIGNAL(timeout()), this, SLOT(updateImageDb()));
//	iutimer->start(500);
	
	
	//setup the File menu list
	menu_file = new QMenu(tr("File"));
	this->menuBar()->addMenu(menu_file);
	//add actions to the file menu
	action_quit = menu_file->addAction(tr("Quit"), this, SLOT(file_quit()));
	
	//the label to display the data rate
	bpslabel = new QLabel(tr("DL: 0"));
	bps = 0;
	//setup timer to calculate datarate
	bpstimeout = new QTimer;
	connect(bpstimeout, SIGNAL(timeout()), this, SLOT(calcData()));
	bpstimeout->start(500);
	
	//setup the Window menu list
	menu_window = new QMenu(tr("Window"));
	this->menuBar()->addMenu(menu_window);
	//setup the windows menu in the main menu bar
	//add actions to the window menu
	action_status = menu_window->addAction(tr("Status"), this, SLOT(window_status()));
	action_images = menu_window->addAction(tr("Images"), this, SLOT(window_images()));
	action_log = menu_window->addAction(tr("Log"), this, SLOT(window_log()));
	action_status->setCheckable(true);
	action_images->setCheckable(true);
	action_log->setCheckable(true);
	action_status->setChecked(true);
	action_images->setChecked(true);
	action_log->setChecked(true);
	
	//setup the variables to be able to convert the iplimage to a qpixmap
	iplconv_init = false;
	dframe = 0;
	tchannel0 = 0;
	tchannel1 = 0;
	tchannel2 = 0;
	tchannel3 = 0;
	
	//this is the label that will be used to display the video stream
	//in the main window
	disp = new QLabel;
	disp->setBackgroundRole(QPalette::Base);
	disp->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	disp->setScaledContents(false);
	setCentralWidget(disp);
	
	//setup the image table
	QStringList header;
	header.append(tr("Name"));
	header.append(tr("Downloaded"));
	header.append(tr("Grounded"));
	header.append(tr("Yaw"));
	header.append(tr("Pitch"));
	header.append(tr("Roll"));
	header.append(tr("X"));
	header.append(tr("Y"));
	header.append(tr("Alt"));
	imagetable = new QTableWidget(0, 9);
	imagetable->setHorizontalHeaderLabels(header);
	//setup the dock to display the image table
	itdock = new QDockWidget(tr("Images"));
	connect(itdock, SIGNAL(visibilityChanged(bool)), this, SLOT(images_vis(bool)));
	itdock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea 
							| Qt::BottomDockWidgetArea);
	itdock->setWidget(imagetable);
	this->addDockWidget(Qt::RightDockWidgetArea, itdock);
	
	
	//add a text box so we can easily change the compression method in realtime
	compress = new QLineEdit;
	updatecomp = new QPushButton(tr("Set Compression"));
	connect(updatecomp, SIGNAL(pressed()), this, SLOT(compPressed()));
	
	//setup the buttons to control the camera
	camstatelabel = new QLabel(tr("Camera: Unknown State"));
	QPushButton* zin = new QPushButton(tr("Zoom In"));
	QPushButton* zout = new QPushButton(tr("Zoom Out"));
	QPushButton* pwr = new QPushButton(tr("Power"));
	QPushButton* cpt = new QPushButton(tr("Capture"));
	QPushButton* dl = new QPushButton(tr("Download"));
	//setup the callbacks
	connect(zin, SIGNAL(pressed()), this, SLOT(camera_zin()));
	connect(zout, SIGNAL(pressed()), this, SLOT(camera_zout()));
	connect(pwr, SIGNAL(pressed()), this, SLOT(camera_power()));
	connect(cpt, SIGNAL(pressed()), this, SLOT(camera_capture()));
	connect(dl, SIGNAL(pressed()), this, SLOT(camera_download()));
	//setup the layout for the buttons
	QHBoxLayout* control1 = new QHBoxLayout;
	QHBoxLayout* control2 = new QHBoxLayout;
	control1->addWidget(zin);
	control1->addWidget(zout);
	control2->addWidget(pwr);
	control2->addWidget(cpt);
	
	//setup the log dock for displaying messages
	logdock = new QDockWidget(tr("Log"));
	QHBoxLayout* logbtnlayout = new QHBoxLayout;
	QPushButton* logsavebtn = new QPushButton(tr("Save Log"));
	QPushButton* logclearbtn = new QPushButton(tr("Clear"));
	connect(logsavebtn, SIGNAL(pressed()), this, SLOT(logSave()));
	connect(logclearbtn, SIGNAL(pressed()), this, SLOT(logClear()));
	logbtnlayout->addWidget(logclearbtn);
	logbtnlayout->addWidget(logsavebtn);
	QVBoxLayout* logrootlayout = new QVBoxLayout;
	logrootlayout->addLayout(logbtnlayout);
	logrootlayout->addWidget(&log);
	QWidget* logwrapper = new QWidget;
	logwrapper->setLayout(logrootlayout);
	logdock->setWidget(logwrapper);
	logdock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea |
							 Qt::BottomDockWidgetArea);
	connect(logdock, SIGNAL(visibilityChanged(bool)), this, SLOT(log_vis(bool)));
	this->addDockWidget(Qt::BottomDockWidgetArea, logdock);
	
	//setup the zoom slider
	zoomslider = new QSlider;
	connect(zoomslider, SIGNAL(valueChanged(int)), this, SLOT(zoomSliderSet(int)));
	zoomslider->setOrientation(Qt::Horizontal);
	zoomslider->setTracking(false);
	zoomslider->setRange(0, 8);
//	zoomslider->setSingleStep(100);
	this->zoompos = 0;
	
	//setup the status dock for display status of devices and stuff
	statusdock = new QDockWidget(tr("Status/Controls"));
	statusdock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	connect(statusdock, SIGNAL(visibilityChanged(bool)), this, SLOT(status_vis(bool)));
	this->addDockWidget(Qt::LeftDockWidgetArea, statusdock);
	
	statlayout = new QVBoxLayout;
	QWidget* widgetwrapper = new QWidget;
	widgetwrapper->setLayout(statlayout);
	cvopstat = new QLabel(tr("Operator: false"));
	cvidstat = new QLabel(tr("Identifier: false"));
	apstat = new QLabel(tr("Auto Pilot: false"));
	ccstat = new QLabel(tr("Cam Control: false"));
	statlayout->addWidget(cvopstat);
	statlayout->addWidget(cvidstat);
	statlayout->addWidget(apstat);
	statlayout->addWidget(ccstat);
	statlayout->addWidget(bpslabel);
	statlayout->addWidget(compress);
	statlayout->addWidget(updatecomp);
	statlayout->addWidget(camstatelabel);
	statlayout->addWidget(zoomslider);
	statlayout->addLayout(control1);
	statlayout->addLayout(control2);
	statlayout->addWidget(dl);
	statusdock->setWidget(widgetwrapper);
	
	//setup networking
	connNotifier = NULL;
	conn = NULL;
	multlisten = new OUdpSocket(this);
	multlisten->errorFunc(bind(&CVOperator::multError, this, _1));
	multlisten->readyReadFunc(bind(&CVOperator::multActivated, this, 0));
	if(!multlisten->listenMulticast(25000, "225.0.0.37")) {
		log<<error <<"Error initializing socket: " <<multlisten->error()
				<<" " <<multlisten->strerror() <<endl;
	}
	
	//load the settings for the window
	this->loadSettings();
	
	gc.setSendHandler(CameraZoomIn, 0);
	gc.setSendHandler(CameraZoomOut, 0);
	gc.setSendHandler(CameraPower, 0);
	gc.setSendHandler(CameraCapture, 0);
	gc.setSendHandler(CameraDownload, 0);
	gc.setSendHandler(ImageDetails, 0);
	
	
	//setup the callbacks for the ground connection
	gc.setRecvHandler(VideoFrame, [&disp](OByteArray ba)->void{
		CvMat* compFrame = NULL;
		
		ba>>compFrame;
		
		IplImage* img = cvDecodeImage(compFrame);
		
		if(img == NULL) return;
		
		//display the image
		disp->setPixmap(QPixmap::fromImage(IplImageToQImage(img), 0));
		
		//release memory we won't be using anymore
		cvReleaseMat(&compFrame);
		cvReleaseImage(&img);
	});
	
	gc.setRecvHandler(ImageDetails, [&db, this](OByteArray ba)->void{
		ba>>db;
		showImageDb(db);
	});
	
}

CVOperator::~CVOperator() {
	delete ui;
}

void CVOperator::closeEvent(QCloseEvent *) {
	this->saveSettings();
}


void CVOperator::logClear() {
	log.clear();
}

void CVOperator::logSave() {
	QString fn = QFileDialog::getSaveFileName(this, tr("Save Log"), "log.txt", 
											  tr("Text File (*.txt)"));
	
	if(fn.isEmpty()) return;
	
	QFile savef(fn);
	if(!savef.open(QIODevice::ReadWrite | QIODevice::Text))
		return;
	
	QString logtext = log.document()->toPlainText();
	QTextStream stream(&savef);
	stream<<logtext;
}

void CVOperator::saveSettings() {
	//save the geometry settings
	QSettings settings("Engineerutopia", "Vision Operator");
	settings.setValue("main_window", this->saveGeometry());
}

void CVOperator::loadSettings() {
	//load the geometry settings if they are available
	QSettings settings("Engineerutopia", "Vision Operator");
	this->restoreGeometry(settings.value("main_window", 
										 this->saveGeometry()).toByteArray());
}

void CVOperator::zoomSliderSet(int val) {
	if(conn) {
		OByteArray pack;
		PacketLength length;
		PacketType type;
		
		int delta = this->zoompos - val;
		if(val == 0) {
			type = CameraZoomOut;
			if(delta < 0) delta *= -1;
			delta += 2;
		} else if(delta > 0) {
			type = CameraZoomOut;
		} else if(delta < 0) {
			type = CameraZoomIn;
			delta *= -1;
		}
		
		pack<<length <<type <<delta;
		pack.seek(0);
		length = pack.size() - sizeof(PacketLength);
		pack<<length;
		
		conn->write(pack);
	}
	this->zoompos = val;
}

void CVOperator::compPressed() {
	if(conn) {
		OByteArray pack;
		
		PacketType type = CompressionMethod;
		PacketLength length;
		OString compmeth = compress->text();
		
		pack<<length <<type <<compmeth;
		
		length = pack.size() - sizeof(PacketLength);
		pack.seek(0);
		pack<<length;
		
		conn->write(pack);
	} else {
		log<<error <<"Error: Not Connected" <<endl;
	}
}

void CVOperator::updateImageDb() {
	this->smallMsg(ImageDetails);
}

void CVOperator::calcData() {
	QString dstring(tr("DL(KB/s): "));
	dstring.append(QString::number(bps/1000/.5));
	
	bpslabel->setText(dstring);
	bps = 0;
}

void CVOperator::camera_zin() {
	if(gc.connected()) {
		OByteArray pack;
		int zoomlen = 1;
		pack<<zoomlen;
		
		gc.write(CameraZoomIn, pack);
	}
	
	zoomslider->setSliderPosition(zoomslider->sliderPosition()+1);
	this->zoompos = zoomslider->sliderPosition();
}

void CVOperator::camera_zout() {
	if(gc.connected()) {
		OByteArray pack;
		int zoomlen = 1;
		pack<<zoomlen;
		
		gc.write(CameraZoomOut, pack);
	}
	
	if(zoomslider->sliderPosition() > 0)
		zoomslider->setSliderPosition(zoomslider->sliderPosition()-1);
	this->zoompos = zoomslider->sliderPosition();
}

void CVOperator::camera_capture() {
	smallMsg(CameraCapture);
	smallMsg(ImageDetails);
}

void CVOperator::camera_power() {
	smallMsg(CameraPower);
}

void CVOperator::camera_download() {
	smallMsg(CameraDownload);
}

void CVOperator::smallMsg(int in) {
	if(gc.connected()) {
		OByteArray data;
		gc.write(in, data);
	}
}

void CVOperator::file_quit() {
	QApplication::quit();
}

void CVOperator::window_status() {
	statusdock->setVisible(!statusdock->isVisible());
}

void CVOperator::window_images() {
	itdock->setVisible(!itdock->isVisible());
}

void CVOperator::window_log() {
	logdock->setVisible(!logdock->isVisible());
}

void CVOperator::status_vis(bool i) {
	action_status->setChecked(i);
}

void CVOperator::images_vis(bool i) {
	action_images->setChecked(i);
}

void CVOperator::log_vis(bool i) {
	action_log->setChecked(i);
}

void CVOperator::multError(OSockError e) {
	log<<error <<"Error: " <<e.code() <<" " <<e.string() <<endl;
}

void CVOperator::connReadyRead() {
	OByteArray data;
	PacketLength length;
	
	do {
		//read in the length field so we know how much data to read in
		//to get the full packet
		data = conn->read(sizeof(PacketLength));
		bps += sizeof(PacketLength);
		
		//extract the field from the serialized data
		length = 0;
		data>>length;
		bps += length;
		
		//check to make sure the data stream is still aligned
		if(length > 1500 || length < 0) {
			log<<error <<"Lost data stream alignment, reconnecting!" <<endl;
			conn->close();
//			conn->connect(addr.ipString(), 25001);
			return;
		} else {
			while(conn->available() < length) {
				usleep(100);
			}
		}
	
		//read in the full packet
		OByteArray pack = conn->read(length);
		
		//handle the packet
		handlePacket(pack);
	} while(conn->available() > 2000);
	
}

void CVOperator::handlePacket(OByteArray &pack) {
	//extract the packet type field so we can process is in the
	//switch statement
	PacketType type;
	pack>>type;
	
	//if a function was not set for the specified type throw an error
#ifndef NDEBUG
	if(!switcher[type]) {
		cout<<"Type: " <<(int)type <<endl;
	}
#endif
	assert(switcher[type]);
	
	//call the handler function for the type of the received packet
	switcher[type](pack);
}

void CVOperator::VideoFrameHeaderSwitch(OByteArray& pack) {
	uint32_t imagecount;
	uint16_t maxlength;
	uint32_t step;
	int32_t rows;
	int32_t cols;
	int32_t type;
	
	pack>>imagecount >>segsPerFrame >>align >>step >>rows >>cols 
			>>type;
	
	currentIndex = imagecount;
	
	compFrame = cvCreateMat(rows, cols, type);
	
	compFrame->step = step;
	compFrame->type = type;
}

void CVOperator::VideoFrameSegmentSwitch(OByteArray& pack) {
	if(skip == currentIndex) {
		return;
	}
	
	uint16_t segindex;
	uint32_t imagecount;
	uint16_t maxlength;
	
	//deserialize the size fields for this image segment
	pack>>segindex >>imagecount >>maxlength;
	
	uchar* ptr;
	try {
		//get a pointer to the data section of the opencv structure
		ptr = cvPtr1D(compFrame,0);	//ptr to the image matrix in memory
	} catch(exception& e) {
		skip = currentIndex;
		return;
	}
	
	//deserialize the image data
	pack.read((char*)ptr + segindex*align, maxlength);
	
	
	//check if this is the last segment, if so display the image
	if((segsPerFrame - 1) == segindex) {
		
		IplImage* img = cvDecodeImage(compFrame);
		
		if(img == NULL) return;
		
		//display the image
		disp->setPixmap(QPixmap::fromImage(IplImageToQImage(img), 0));
		
		//release memory we won't be using anymore
		cvReleaseMat(&compFrame);
		cvReleaseImage(&img);
	}
}

void CVOperator::ImageDetailsSwitch(OByteArray& pack) {
	pack>>db;
	
	showImageDb(db);
}

void CVOperator::ErrorMsgSwitch(OByteArray& pack) {
	OString msg;
	pack>>msg;
	log<<error <<msg <<endl;
}

void CVOperator::CameraStatusSwitch(OByteArray& pack) {
	int camera_state;
	pack>>camera_state;
	switch(camera_state) {
	case Camera::Ready: {
			camstatelabel->setText(tr("Camera: Ready"));
			break;
		}
	case Camera::Sleeping: {
			camstatelabel->setText(tr("Camera: Sleeping"));
			break;
		}
	case Camera::PowerOff: {
			camstatelabel->setText(tr("Camera: Off"));
			break;
		}
	case Camera::Downloading: {
			camstatelabel->setText(tr("Camera: Downloading"));
			break;
		}
	case Camera::Capturing: {
			camstatelabel->setText(tr("Camera: Capturing"));
			break;
		}
	case Camera::Zooming: {
			camstatelabel->setText(tr("Camera: Zooming"));
			break;
		}
	case Camera::Connected: {
			this->setCamControlStatus(true);
			break;
		}
	case Camera::Disconnected: {
			this->setCamControlStatus(false);
			break;
		}
	default: {
			log<<error <<"Trying to set unknown camera state \"" <<(int)camera_state <<"\"" <<endl;
			break;
		}
	}
}

void CVOperator::multActivated(int) {
	log<<"Got Multicast Packet" <<endl;
	
	OByteArray data = multlisten->readAll(addr);
	
	if(!gc.connected()) {
		log<<"New Connection to Plane" <<endl;
		setCVOperatorStatus(true);
		
		addr.port(25001);
		gc.connect(addr);
	}
}

void CVOperator::connActivated(int) {
	if(conn) {
		conn->errorLoop();
		conn->readLoop();
	}
}

void CVOperator::connDisconnected() {
	log<<error <<"Connection to plane closed!" <<endl;
	
	delete connNotifier;
	delete conn;
	connNotifier = NULL;
	conn = NULL;
}

void CVOperator::changeEvent(QEvent *e) {
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void CVOperator::setCVOperatorStatus(bool stat) {
	if(stat)
		cvopstat->setText(tr("Operator: true"));
	else
		cvopstat->setText(tr("Operator: false"));
}

void CVOperator::setCVIdentifierStatus(bool stat) {
	if(stat)
		cvidstat->setText(tr("Identifier: true"));
	else
		cvidstat->setText(tr("Identifier: false"));
}

void CVOperator::setAutoPilotStatus(bool stat) {
	if(stat)
		apstat->setText(tr("Auto Pilot: true"));
	else
		apstat->setText(tr("Auto Pilot: false"));
}

void CVOperator::setCamControlStatus(bool stat) {
	if(stat)
		ccstat->setText(tr("Cam Control: true"));
	else
		ccstat->setText(tr("Cam Control: false"));
}

void CVOperator::showImageDb(ImageDatabase &db) {
	imagetable->setRowCount(db.size());
	
	for(int i=0; i<db.size(); i++) {
		QString str;
		
		imagetable->setItem(i, 0, new QTableWidgetItem(QString(db[i].i_name.toCString())));
		imagetable->setItem(i, 1, new QTableWidgetItem(str.setNum(db[i].i_downloaded)));
		imagetable->setItem(i, 2, new QTableWidgetItem(str.setNum(db[i].i_grounded)));
		imagetable->setItem(i, 3, new QTableWidgetItem(str.setNum(db[i].i_yaw)));
		imagetable->setItem(i, 4, new QTableWidgetItem(str.setNum(db[i].i_pitch)));
		imagetable->setItem(i, 5, new QTableWidgetItem(str.setNum(db[i].i_roll)));
		imagetable->setItem(i, 6, new QTableWidgetItem(str.setNum(db[i].i_x)));
		imagetable->setItem(i, 7, new QTableWidgetItem(str.setNum(db[i].i_y)));
		imagetable->setItem(i, 8, new QTableWidgetItem(str.setNum(db[i].i_alt)));
	}
}

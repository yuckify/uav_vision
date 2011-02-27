#include "CVOperator.h"
#include "ui_CVOperator.h"

void DebugMsg(OString str) {
#ifdef DEBUG
	cout<<str <<endl;
#endif
}

CVOperator::CVOperator(QWidget *parent) : 
	QMainWindow(parent), ui(new Ui::CVOperator) {
	ui->setupUi(this);
	skip = -1;
	
	//setup the File menu list
	menu_file = new QMenu(tr("File"));
	this->menuBar()->addMenu(menu_file);
	//add actions to the file menu
	action_quit = menu_file->addAction(tr("Quit"), this, SLOT(file_quit()));
	
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
	logdock->setWidget(&log);
	logdock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea |
							 Qt::BottomDockWidgetArea);
	connect(logdock, SIGNAL(visibilityChanged(bool)), this, SLOT(log_vis(bool)));
	this->addDockWidget(Qt::BottomDockWidgetArea, logdock);
	
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
	statlayout->addWidget(compress);
	statlayout->addWidget(updatecomp);
	statlayout->addWidget(camstatelabel);
	statlayout->addLayout(control1);
	statlayout->addLayout(control2);
	statlayout->addWidget(dl);
	statusdock->setWidget(widgetwrapper);
	
	
	//setup networking
	conn = NULL;
	multlisten = new OUdpSocket();
	multlisten->errorFunc(bind(&CVOperator::multError, this, _1));
	multlisten->listenMulticast(25000, "225.0.0.37");
	log<<error <<"error: " <<multlisten->error() <<" " <<multlisten->strerror() <<endl;
	log<<normal <<"Multicast FD: " <<multlisten->fileDescriptor() <<endl;
	multNotifier = new QSocketNotifier(multlisten->fileDescriptor(), 
									   QSocketNotifier::Read, this);
	
	log<<error <<"error: " <<multlisten->error() <<endl;
	
	connect(multNotifier, SIGNAL(activated(int)), this, SLOT(multActivated(int)));
	
}

CVOperator::~CVOperator() {
	delete ui;
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

void CVOperator::camera_zin() {
	smallMsg(CameraZoomIn);
}

void CVOperator::camera_zout() {
	smallMsg(CameraZoomOut);
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
	if(conn) {
		PacketType type = in;
		PacketLength length = 0;
		
		OByteArray pack;
		pack<<length <<type;
		
		length = pack.size() - sizeof(PacketLength);
		pack.seek(0);
		pack<<length;
		
		conn->write(pack);
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
	log<<error <<e.code() <<" " <<e.string() <<endl;
}

void CVOperator::connReadyRead() {
	
	//read in the length field so we know how much data to read in
	//to get the full packet
	OByteArray data = conn->read(sizeof(PacketLength));
	
	//extract the field from the serialized data
	PacketLength length = 0;
	data>>length;
	
	while(conn->available() < length) {
		usleep(100);
	}
	
	//read in the full packet
	OByteArray pack = conn->read(length);
	
	//extract the packet type field so we can process is in the
	//switch statement
	PacketType type;
	pack>>type;
	
	
	switch(type) {
	case VideoFrameHeader: {
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
			
			break;
		}
	case VideoFrameSegment: {
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
				
				
				CvSize frame_size;
				frame_size.height = img->height;
				frame_size.width = img->width;
				
				if(!iplconv_init || dframe->width != img->width || 
						dframe->height != img->height) {
					
					if(dframe) {
						cvReleaseImage(&dframe);
						cvReleaseImage(&tchannel0);
						cvReleaseImage(&tchannel1);
						cvReleaseImage(&tchannel2);
						cvReleaseImage(&tchannel3);
					}
					
					dframe = cvCreateImage(frame_size, img->depth, 4);
					
					// the individual channels for the IplImage
					tchannel0 = cvCreateImage(frame_size, IPL_DEPTH_8U, 1);
					tchannel1 = cvCreateImage(frame_size, IPL_DEPTH_8U, 1);
					tchannel2 = cvCreateImage(frame_size, IPL_DEPTH_8U, 1);
					tchannel3 = cvCreateImage(frame_size, IPL_DEPTH_8U, 1);
					
					iplconv_init = true;
				}
				
				// set all elements in tchannel0 (alpha channel) to 255
				cvSet(tchannel0,cvScalarAll(255),0);
				
				// with img being the captured frame (3 channel RGB)
				// and dframe the frame to be displayed
				cvSplit(img, tchannel1, tchannel2, tchannel3, NULL);
				cvMerge(tchannel1, tchannel2, tchannel3, tchannel0, dframe);
				
				// point to the image data stored in the IplImage*
				const unsigned char * data = (unsigned char *)(dframe->imageData);
				
				// read other parameters in local variables
				int width = dframe->width;
				int height = dframe->height;
				int bytesPerLine = dframe->widthStep;
				
				disp->clear();
				
				// imageframe is my QLabel object
				QImage qimage = QImage(data, width, height, bytesPerLine, QImage::Format_RGB32 );
				disp->setPixmap(QPixmap::fromImage(qimage, 0));
				
				cvReleaseMat(&compFrame);
				cvReleaseImage(&img);
//				cvReleaseImage(&dframe);
//				cvReleaseImage(&tchannel0);
//				cvReleaseImage(&tchannel1);
//				cvReleaseImage(&tchannel2);
//				cvReleaseImage(&tchannel3);
				
			}
			break;
		}
	case ImageDetails: {
			pack>>db;
			
			showImageDb(db);
			
			break;
		}
	case ErrorMsg: {
			OString msg;
			pack>>msg;
			log<<error <<msg <<endl;
			break;
		}
	case CameraStatus: {
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
					cerr<<"Trying to set unknown camera state." <<endl;
					break;
				}
			}
			
			break;
		}
	}
	
}

void CVOperator::multActivated(int) {
	multNotifier->setEnabled(false);
	
	DebugMsg("Got Multicast Packet");
	OSockAddress addr;
	
	OByteArray data = multlisten->readAll(addr);
	
	if(!conn) {
		DebugMsg("New GTcpConnection to Plane");
		setCVOperatorStatus(true);
		
		conn = new OTcpSocket();
		conn->connect(addr.ipString(), 25001);
		conn->readyReadFunc(bind(&CVOperator::connReadyRead, this));
		conn->disconnectFunc(bind(&CVOperator::connDisconnected, this));
		connNotifier = new QSocketNotifier(conn->fileDescriptor(),
										   QSocketNotifier::Read, this);
		connect(connNotifier, SIGNAL(activated(int)), this, SLOT(connActivated(int)));
	}
	multNotifier->setEnabled(true);
}

void CVOperator::connActivated(int) {
	if(conn) {
		conn->readLoop();
	}
}

void CVOperator::connDisconnected() {
	log<<error <<"disconnected" <<endl;
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

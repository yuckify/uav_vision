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
	
	//setup the capture thread to do the actual capturing
	
//	imgview = new QGraphicsView;
//	imgscene = new QGraphicsScene;
//	imgview->setScene(imgscene);
	
	cvNamedWindow("window");
	
	disp = new QLabel;
	disp->setBackgroundRole(QPalette::Base);
	disp->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	disp->setScaledContents(true);
	setCentralWidget(disp);
	
	//add a text box so we can easily change the compression method in realtime
	compress = new QLineEdit;
	updatecomp = new QPushButton(tr("Set Compression"));
	connect(updatecomp, SIGNAL(pressed()), this, SLOT(compPressed()));
	
	//setup the status dock for display status of devices and stuff
	statusdock = new QDockWidget(tr("Status"));
	statusdock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	this->addDockWidget(Qt::LeftDockWidgetArea, statusdock);
	
	statlayout = new QVBoxLayout;
	QWidget* widgetwrapper = new QWidget;
	widgetwrapper->setLayout(statlayout);
	cvopstat = new QLabel(tr("Operator: false"));
	cvidstat = new QLabel(tr("Identifier: false"));
	apstat = new QLabel(tr("Auto Pilot: false"));
	statlayout->addWidget(cvopstat);
	statlayout->addWidget(cvidstat);
	statlayout->addWidget(apstat);
	statlayout->addWidget(compress);
	statlayout->addWidget(updatecomp);
	statusdock->setWidget(widgetwrapper);
	
	
	//setup networking
	conn = NULL;
	multlisten = new OUdpSocket();
	multlisten->errorFunc(bind(&CVOperator::multError, this, _1));
	multlisten->listenMulticast(25000, "225.0.0.37");
	cout<<"error: " <<multlisten->error() <<" " <<multlisten->strerror() <<endl;
	cout<<"Multicast FD: " <<multlisten->fileDescriptor() <<endl;
	multNotifier = new QSocketNotifier(multlisten->fileDescriptor(), 
									   QSocketNotifier::Read, this);
	
	cout<<"error: " <<multlisten->error() <<endl;
	
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
		cout<<"Error: Not Connected" <<endl;
	}
}

void CVOperator::multError(OSockError e) {
	cout<<e.code() <<" " <<e.string() <<endl;
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
				
				cvShowImage("window", img);
				cvWaitKey(1);
				
				cvReleaseMat(&compFrame);
				cvReleaseImage(&img);
				
			}
			break;
		}
	case ImageDetails: {
			pack>>db;
			
			break;
		}
	case ErrorMsg: {
			OString msg;
			pack>>msg;
			cout<<msg <<endl;
			break;
		}
	}
	
	prev = pack;
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
	cout<<"disconnected" <<endl;
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

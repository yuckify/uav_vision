#include"VideoThread.h"

VideoThread::VideoThread(bst::mutex &mut, deque<OByteArray> &pkts, OPipe &p, PlaneInfo& inf) :
		videolock(mut), videopacks(pkts), info(inf) {
	//setup the pipe so we can a signal to the main thread that
	//the video data is available
	pipe = new OPipe(p);
	
	//setup the callback that will be executed when this thread is started
//	this->connect(bind(&VideoThread::run, this));
	this->callback(bind(&VideoThread::run, this));
	
	//the default compression to use for the live video stream
	compext = ".jpeg";
}

void VideoThread::run() {
	CvCapture* capture = cvCaptureFromCAM(-1);
	if(!capture) {
		cout<<"Error Opening Camera" <<endl;
	}
	IplImage* frame;
	int p[3];
	p[0] = CV_IMWRITE_JPEG_QUALITY;
	p[1] = 100;
	p[2] = 0;
	uint32_t imagecount=0;
	
	while(1) {
		//Grab Frames
		frame = cvQueryFrame( capture );
		
		//make sure the video buffer does not already have data in it
		//if it does drop this frame to make sure the buffer does not
		//overflow
		if(videopacks.size()) {
			OByteArray tmp("0", 1);
			pipe->write(tmp);
			continue;
		}
		
		if( !frame ) {
			cout<<"Error Capturing Frame" <<endl;
			break;
		}
		
		comptex.lock();
		CvMat* temp;
		
		//if the new compression method fails, reset to jpeg
		try {
			temp = cvEncodeImage(compext.toCString(), frame, p); //Encode image as Jpeg
		} catch(cv::Exception e) {
			//write the message to the pipe, the main thread then gets the message
			//and writes it to the socket so it may be displayed on the ground
			OString msg;
			msg<<"Error: Compression method \'" <<compext <<"\' is not supported, "
					<<"reseting to default \'.jpeg\'";
			OByteArray data(msg);
			pipe->write(data);
			
			compext = ".jpeg";
			temp = cvEncodeImage(compext.toCString(), frame, p); //Encode image as Jpeg
			
		}
		comptex.unlock();
		
		uint16_t count=0; //image piece counter
		int headersize=sizeof(int)*4; //header size
		uint16_t maxlength=1024-headersize; //1024 - the 10 block header
		uint16_t iterations=((temp->step)/maxlength); //Number of blocks in split photo into this many blocks
		
		int offset=maxlength;
		if (temp->step%offset!=0)iterations++; //Assures that iterations is proper size
		
		PacketLength length;
		PacketType type = VideoFrameHeader;
		
		OByteArray pack;
		pack<<length <<type <<imagecount <<iterations <<maxlength <<temp->step
				<<temp->rows <<temp->cols <<temp->type;
		
		length = pack.size() - sizeof(PacketLength);
		pack.seek(0);
		pack<<length;
		
		//lock the mutex and push the packet into the queue
		videolock.lock();
		videopacks.push_back(pack);
		
		type = VideoFrameSegment;
		for (int j=0;j<iterations;j++)  //Loop to split data into blocks
		{
			if (j==iterations-1){maxlength=((temp->step)%maxlength);}  //If last time through loop. Sets to endsize
			uchar* ptr=cvPtr1D(temp,0);	//ptr to the image matrix in memory
			
			OByteArray spack;
			spack<<length <<type <<count <<imagecount <<maxlength;
			spack.append((char*)ptr+(j*offset),maxlength);
			length = spack.size() - sizeof(PacketLength);
			spack.seek(0);
			spack<<length;
			
			count++; //Count for the number of pieces saved as an int, doubles as piece number
			videopacks.push_back(spack);//push the video segment into the network buffer
		}
		videolock.unlock();
		imagecount++;
		
		cvReleaseMat(&temp);
		
		//signal the main thread that this new image data is available
		OByteArray tmp;
		tmp<<(uint8_t)50;
		pipe->write(tmp);
		
	}//end while(1)
}

void VideoThread::setCompression(OString comp) {
	unique_lock<bst::mutex> locker(comptex);
	compext = comp;
	cout<<"Setting Compression \"" <<comp <<"\"" <<endl;
}

#include"VideoThread.h"

VideoThread::VideoThread(ODataStream<>* strm, PlaneInfo& inf) :
		info(inf) {
	stream = strm;
	
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
	
	while(1) {
		if(!stream->socket().connected()) {
			usleep(100000);
			continue;
		}
		
		//Grab Frames
		frame = cvQueryFrame(capture);
		
		if(!frame) {
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
			
			compext = ".jpeg";
			temp = cvEncodeImage(compext.toCString(), frame, p); //Encode image as Jpeg
			
		}
		comptex.unlock();
		
		//write the frame to the data stream
		if(stream->socket().connected()) {
			OByteArray data;
			data<<temp;
			stream->write(VideoFrame, data);
		}
		cvReleaseMat(&temp);
	}//end while(1)
}

void VideoThread::setCompression(OString comp) {
	unique_lock<bst::mutex> locker(comptex);
	compext = comp;
	cout<<"Setting Compression \"" <<comp <<"\"" <<endl;
}

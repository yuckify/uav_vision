#ifndef VIDEOTHREAD_H
#define VIDEOTHREAD_H

#include<sys/types.h>
#include<iostream>

#include<boost/thread/mutex.hpp>

namespace bst = boost;

#include<queue>

#include<opencv/highgui.h>
#include<opencv/cv.h>
#include<opencv/cxcore.h>

#include<OPipe.hpp>
#include<OByteArray.hpp>
#include<ODataStream.hpp>
#include<OCompatibility.hpp>

#include<Defs.h>

using namespace std;

class VideoThread : public OThread {
public:
	VideoThread(ODataStream<>* strm, PlaneInfo& inf);
	
	void run();
	
	void setCompression(OString comp);
	
private:
	ODataStream<>* stream;
	PlaneInfo& info;
	//compression extension
	bst::mutex comptex;
	OString compext;
};



#endif // VIDEOTHREAD_H

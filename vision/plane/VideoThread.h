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
	VideoThread(ODataStreamBase* strm, bst::mutex& mut, 
				deque<OByteArray>& pkts, OPipe& p, PlaneInfo& inf);
	
	void run();
	
	void setCompression(OString comp);
	
private:
	ODataStreamBase* stream;
	OPipe* pipe;
	bst::mutex& videolock;
	deque<OByteArray>& videopacks;
	PlaneInfo& info;
	//compression extension
	bst::mutex comptex;
	OString compext;
};



#endif // VIDEOTHREAD_H

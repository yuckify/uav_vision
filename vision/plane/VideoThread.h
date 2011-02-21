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

#include<Defs.h>

using namespace std;

class VideoThread : public OThread {
public:
	VideoThread(bst::mutex& mut, queue<OByteArray>& pkts, OPipe& p, PlaneInfo& inf);
	
	void run();
	
	void setCompression(OString comp);
	
private:
	OPipe* pipe;
	bst::mutex& videolock;
	queue<OByteArray>& videopacks;
	PlaneInfo& info;
	//compression extension
	bst::mutex comptex;
	OString compext;
};



#endif // VIDEOTHREAD_H

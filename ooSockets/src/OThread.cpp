#include"OThread.hpp"

OThread::OThread() {
	is_running = false;
	clear();
}

OThread::OThread(function<void ()> cbk) {
	is_running = false;
	clear();
	
	callback(cbk);
	start();
}

OThread::~OThread() {
#ifdef __windows__

#else
	if(isRunning())
		pthread_cancel(thread);
#endif
	
}

void OThread::start() {
#ifdef __windows__
	thread = CreateThread(NULL, 0, OThread::runThread, (LPVOID)this, 0, NULL);
#else
	pthread_create(&thread, 0, &OThread::runThread, (void*)this);
#endif
}

void OThread::exec() {
	while(execOnce());
}

void OThread::clear() {
	readMap.clear();
	priorityMap.clear();
	writeMap.clear();
	deltalist.clear();
	fdmax = 0;
	FD_ZERO(&readfds);
	FD_ZERO(&writefds);
	FD_ZERO(&priorityfds);
	runCbk = NULL;
}

bool OThread::execOnce() {
#ifdef __windows__
	if(tout == INFINITE && !readfds.size()) {
		return false;
	}

	//block on the reading fds
	DWORD ret = WaitForMultipleObjects(readfds.size(), &(*readfds.begin()), false, tout);
#else
	//if there are no more file descriptors and the timeout
	//is not set, exit the run loop since there is nothing to do
	if(!readMap.size() && !writeMap.size() && !priorityMap.size() && !tout.get()) {
		return false;
	}

	fd_set tmpreadset = readfds;
	fd_set tmpwriteset = writefds;
	fd_set tmppriorityset = priorityfds;
	
	//block on the reading fds
	int ret = select(fdmax + 1, &tmpreadset, &tmpwriteset, &tmppriorityset, tout.get());
#endif
	
	//handle any timers that may have expired
#ifdef __windows__
	if(ret == WAIT_TIMEOUT && tout != INFINITE) {
#else
	if(!ret && tout.get()) {
#endif
		//adjust the delta values in the list
		for(auto i=deltalist.end(); i>=deltalist.begin(); i--) {
			i->delta -= deltalist[0].delta;
		}
		
		bool repeat = false;
		do {
			//call the callback for the current timer
			if(deltalist[0].delta.usec() <= 0) {
				deltalist[0].timer->runLoop();
			} else {
				break;
			}
			
			//if the timer that was just executed was a single
			//fire timer remove it from the list
			if(deltalist[0].timer->type() == OO::Once) {
				deltalist.removeAt(0);
			} else {
				//reset the timer that timed out
				deltalist[0].delta = deltalist[0].timer->period();
			}
			
			//resort the delta list
			sort(deltalist.begin(), deltalist.end(),
				 [] (const TimerDelta& a, const TimerDelta& b) -> bool {
				return a.delta < b.delta;
			});
			
			if(deltalist.size()) {
				if(deltalist[0].delta <= 0)
					repeat = true;
				else
					repeat = false;
			}
			
		} while(repeat);
		
		//check to see if we have any timers left to handle
#ifdef __windows__
		if(deltalist.size()) {
			//get the current timeout
			tout = deltalist[0].timer->period().msec();
		} else {
			//no timers left so set tout to INFINITE to signal the
			//blocking function to wait infinitely
			tout = INFINITE;
		}
#else
		if(deltalist.size()) {
			//get the current timeout
			*tout = deltalist[0].delta.toTimeval();//.timer->period().toTimeval();
		} else {
			//no timers left so get rid of the timeval structure
			tout.reset();
		}
#endif
	}//end if(timeout)
	
	OTime bfd;
	bfd.setCurrent();
	
#ifdef __windows__
	//handle any HANDLEs that have activity
	for(auto i=readfds.begin() + ret - WAIT_ABANDONED_0; i<readfds.end(); i++) {
		for(auto j=readMap.begin(); j<readMap.end(); j++) {
			if(*i == j->fd) {
				j->obj->readLoop();
			}
		}
	}

#else
	//handle the fds with pending errors
	unsigned length = priorityMap.size();
	for(unsigned i=0; i<length; i++) {
		//if the fd is set then call the runloop for the associated object
		if(FD_ISSET(priorityMap[i].fd, &tmppriorityset)) {
			priorityMap[i].obj->priorityLoop();
		}
	}
	
	//handle the fds ready to be read from
	length = readMap.size();
	for(unsigned i=0; i<length; i++) {
		//if the fd is set then call the runloop for the associated object
		if(FD_ISSET(readMap[i].fd, &tmpreadset)) {
			readMap[i].obj->readLoop();
		}
	}
	
	//handle the fds ready to be written to
	length = writeMap.size();
	for(unsigned i=0; i<length; i++) {
		//if the fd is set then call the runloop for the associated object
		if(FD_ISSET(writeMap[i].fd, &tmpwriteset)) {
			writeMap[i].obj->writeLoop();
		}
	}
#endif
	
	OTime efd;
	efd.setCurrent();
	
	OTime dfd = efd - bfd;
	for(auto i=deltalist.begin(); i<deltalist.end(); i++) {
		i->delta -= efd;
	}
	
	return true;
}

void OThread::registerReadFD(OO::HANDLE fd, OIODevice* o) {
	//first make sure we are working with a valid file descriptor
	if(fd <= 0) return;
	
	fdMap xfer(fd, o);
	
	//check to make sure the file descriptor is not already added
	for(unsigned i=0; i<readMap.size(); i++) {
		//the file descriptor matches, so just reset the object
		//pointer and we are done here
		if(readMap[i].fd == fd) {
			readMap[i].obj = o;
			return;
		}
	}
	
	readMap.push_back(xfer);
	
#ifdef __windows__
	readfds.push_back(fd);
#else
	FD_SET(fd, &readfds);
	
	if(fd > fdmax) fdmax = fd;
#endif
}

void OThread::registerWriteFD(OO::HANDLE fd, OIODevice *o) {
	//first make sure we are working with a valid file descriptor
	if(fd <= 0) return;
	
	fdMap xfer(fd, o);
	
	//check to make sure the file descriptor is not already added
	for(unsigned i=0; i<writeMap.size(); i++) {
		//the file descriptor matches, so just reset the object
		//pointer and we are done here
		if(writeMap[i].fd == fd) {
			writeMap[i].obj = o;
			return;
		}
	}
	
	writeMap.push_back(xfer);
	
#ifdef __windows__

#else
	FD_SET(fd, &writefds);
#endif
}

void OThread::registerPriorityFD(OO::HANDLE fd, OIODevice* o) {
	//first make sure we are working with a valid file descriptor
	if(fd <= 0) return;
	
	fdMap xfer(fd, o);
	
	//check to make sure the file descriptor is not already added
	for(unsigned i=0; i<priorityMap.size(); i++) {
		//the file descriptor matches, so just reset the object
		//pointer and we are done here
		if(priorityMap[i].fd == fd) {
			priorityMap[i].obj = o;
			return;
		}
	}
	
	priorityMap.push_back(xfer);
	
#ifdef __windows__
	
#else
	FD_SET(fd, &priorityfds);
#endif
}

void OThread::unregisterReadFD(OO::HANDLE fd) {
	//find the fd in the map and remove it
	unsigned length = readMap.size();
	for(unsigned i=0; i<length; i++) {
		if(readMap[i].fd == fd) {
			readMap.erase(readMap.begin() + i);
			break;
		}
	}
	
	//get the fd out of the fd_set
#ifdef __windows__

#else
	FD_CLR(fd, &readfds);
#endif
}

void OThread::unregisterWriteFD(OO::HANDLE fd) {
	//find the fd in the write map and remove it
	unsigned length = writeMap.size();
	for(unsigned i=0; i<length; i++) {
		if(writeMap[i].fd == fd) {
			writeMap.removeAt(i);
			break;
		}
	}
	
	//get the fd out of the fd_set
#ifdef __windows__

#else
	FD_CLR(fd, &writefds);
#endif
}

void OThread::unregisterPriorityFD(OO::HANDLE fd) {
	//find the fd in the write map and remove it
	unsigned length = priorityMap.size();
	for(unsigned i=0; i<length; i++) {
		if(priorityMap[i].fd == fd) {
			priorityMap.removeAt(i);
			break;
		}
	}
	
	//get the fd out of the fd_set
#ifdef __windows__

#else
	FD_CLR(fd, &priorityfds);
#endif
}

void OThread::registerTimer(OTimerBase* tim) {
	//register this new timer in the delta list
	deltalist.push_back(TimerDelta(tim));
	
	//sort the list according to the timeout period
	sort(deltalist.begin(), deltalist.end(), 
		 [] (const TimerDelta& a, 
			 const TimerDelta& b) -> bool {
		return a.delta < b.delta;
	});
	
#ifdef __windows__
	tout = deltalist[0].timer->period().msec();
#else
	if(!tout.get()) tout.reset(new timeval);
	
	*tout = deltalist[0].timer->period().toTimeval();
#endif
}

void OThread::unregisterTimer(OTimerBase *tim) {
	for(unsigned i=0; i<deltalist.size(); i++) {
		if(deltalist[i].timer == tim) {
			deltalist.removeAt(i);
			break;
		}
	}
	
#ifdef __windows__
	if(deltalist.size() == 0) tout = INFINITE;
#else
	if(deltalist.size() == 0) if(tout.get()) tout.reset();
#endif
	
}

void OThread::callback(function<void ()> cbk) {
    runCbk = cbk;
}

int OThread::processors() {
#if defined(_WIN32) || defined(_WIN64)
	SYSTEM_INFO sysinfo;
	GetSystemInfo( &sysinfo );
	
	return sysinfo.dwNumberOfProcessors;
#elif defined(__linux__) || defined(_AIX32)
	return sysconf( _SC_NPROCESSORS_ONLN );
#elif defined(__apple__)
	int numCPU;
	int mib[4];
	size_t len; 
	
	// set the mib for hw.ncpu
	mib[0] = CTL_HW;
	mib[1] = HW_AVAILCPU;  // alternatively, try HW_NCPU;
	
	// get the number of CPUs from the system
	sysctl(mib, 2, &numCPU, &len, NULL, 0);
	
	if(numCPU < 1) {
		 mib[1] = HW_NCPU;
		 sysctl(mib, 2, &numCPU, &len, NULL, 0);
	
		 if(numCPU < 1) numCPU = 1;
	}
		return numCPU;
#endif
}

bool OThread::isRunning() const {
	return is_running;
}

#ifdef __windows__
DWORD WINAPI OThread::runThread(LPVOID arg) {
	OThread* thr = (OThread*)arg;
	thr->sigRun();
	return NULL;
}
#else
void* OThread::runThread(void* arg) {
	OThread* thr = (OThread*)arg;
	thr->sigRun();
	return NULL;
}

#endif

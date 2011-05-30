#include"OThread.hpp"

OThread::OThread() {
	fdm.reset(NULL);
	clear();
}

OThread::OThread(OThread &other) {
	thread = other.thread;
	fdm = other.fdm;
//	fdm = (boost::scoped_ptr<OThreadFds>&&)other.fdm;
}

OThread::OThread(function<void ()> cbk) {
	fdm.reset(NULL);
	clear();
	
	callback(cbk);
	start();
}

OThread::~OThread() {
	cancel();
}

OThread OThread::self() {
#ifdef __windows__
	
#else
	OThread othr;
	othr.thread = pthread_self();
	return othr;
#endif
}

bool OThread::isSelf() const {
#ifdef __windows__
	
#else
	return thread == pthread_self();
#endif
}

bool OThread::operator ==(OThread& thr) const {
	return this->thread == thr.thread;
}

void OThread::cancel() {
#ifdef __windows__

#else
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
	thread = 0;
	runCbk = NULL;
}

bool OThread::execOnce() {
	if(!fdm.get()) return false;
	
#ifdef __windows__
	if(fdm->tout == INFINITE && !fdm->readfds.size()) {
		return false;
	}

	//block on the reading fds
	DWORD ret = WaitForMultipleObjects(fdm->readfds.size(), 
									   &(*fdm->readfds.begin()), 
									   false, fdm->tout);
#else
	//if there are no more file descriptors and the timeout
	//is not set, exit the run loop since there is nothing to do
	if(fdm->readfds.isEmpty() && 
	   fdm->writefds.isEmpty() && 
	   fdm->errorfds.isEmpty() && 
	   !fdm->tout.get()) {
		return false;
	}

	fd_set tmpreadset = fdm->readfds.fdSet();
	fd_set tmpwriteset = fdm->writefds.fdSet();
	fd_set tmperrorset = fdm->errorfds.fdSet();
	
	//block on the reading fds
	int ret = select(fdm->fdmax + 1, &tmpreadset, 
					 &tmpwriteset, &tmperrorset, 
					 fdm->tout.get());
	
#endif
	
	//handle any timers that may have expired
#ifdef __windows__
	if(ret == WAIT_TIMEOUT && fdm->tout != INFINITE) {
#else
	if(!ret && fdm->tout.get()) {
#endif
		//adjust the delta values in the list
		for(typename OList<TimerDelta>::iterator i=fdm->deltalist.begin(); 
		i<fdm->deltalist.end(); i++) {
			i->delta -= fdm->deltalist[0].delta;
		}
		
		bool repeat = false;
		do {
			//call the callback for the current timer
			if(fdm->deltalist[0].delta.usec() <= 0) {
				fdm->deltalist[0].timer->runLoop();
			} else {
				break;
			}
			
			//if the timer that was just executed was a single
			//fire timer remove it from the list
			if(fdm->deltalist[0].timer->type() == OO::Once) {
				fdm->deltalist.removeAt(0);
			} else {
				//reset the timer that timed out
				fdm->deltalist[0].delta = 
						fdm->deltalist[0].timer->period();
			}
			
			//resort the delta list
			/*
			sort(fdm->deltalist.begin(), fdm->deltalist.end(),
				 [] (const TimerDelta& a, const TimerDelta& b) -> bool {
				return a.delta < b.delta;
			});
			*/
			sort(fdm->deltalist.begin(), fdm->deltalist.end(), boost::bind(deltaSort, _1, _2));
			
			if(fdm->deltalist.size()) {
				if(fdm->deltalist[0].delta <= 0)
					repeat = true;
				else
					repeat = false;
			}
			
		} while(repeat);
		
		//check to see if we have any timers left to handle
#ifdef __windows__
		if(fdm->deltalist.size()) {
			//get the current timeout
			fdm->tout = fdm->deltalist[0].timer->period().msec();
		} else {
			//no timers left so set tout to INFINITE to signal the
			//blocking function to wait infinitely
			fdm->tout = INFINITE;
		}
#else
		if(fdm->deltalist.size()) {
			//get the current timeout
			*fdm->tout = 
					fdm->deltalist[0].delta.toTimeval();//.timer->period().toTimeval();
		} else {
			//no timers left so get rid of the timeval structure
			fdm->tout.reset();
		}
#endif
	}//end if(timeout)
	
#ifdef __windows__
	//handle any HANDLEs that have activity
	for(auto i=fdm->readfds.begin() + ret - WAIT_ABANDONED_0; 
	i<fdm->readfds.end(); i++) {
		for(auto j=fdm->readMap.begin(); j<fdm->readMap.end(); j++) {
			if(*i == j->fd) {
				j->obj->readLoop();
			}
		}
	}

#else
	//handle the fds with pending errors
	unsigned length = fdm->errorfds.size();
	for(OO::HANDLE i=fdm->fdmin; unsigned(i)<length; i++) {
		//if the fd is set then call the runloop for the associated object
		if(FD_ISSET(i, &tmperrorset)) {
			if(fdm->errorfds[i]) fdm->errorfds[i]->priorityLoop();
		}
	}
	
	//handle the fds ready to be read from
	length = fdm->readfds.size();
	for(OO::HANDLE i=fdm->fdmin; unsigned(i)<length; i++) {
		//if the fd is set then call the runloop for the associated object
		if(FD_ISSET(i, &tmpreadset)) {
			if(fdm->readfds[i]) fdm->readfds[i]->readLoop();
		}
	}
	//handle the fds ready to be written to
	length = fdm->writefds.size();
	for(OO::HANDLE i=fdm->fdmin; unsigned(i)<length; i++) {
		//if the fd is set then call the runloop for the associated object
		if(FD_ISSET(i, &tmpwriteset)) {
			if(fdm->writefds[i]) fdm->writefds[i]->writeLoop();
		}
	}
	
#endif
	
	return true;
}

void OThread::allocThreadFd() {
	if(!fdm.get()) fdm.reset(new OThreadFds);
}

void OThread::recalcMinMax(OO::HANDLE fd) {
	if(fd == fdm->fdmin) {
		OO::HANDLE mina = fdm->readfds.min();
		OO::HANDLE minb = fdm->writefds.min();
		OO::HANDLE minc = fdm->errorfds.min();
		fdm->fdmin = ::min(mina, ::min(minb, minc));
	} else if(fd == fdm->fdmax-1) {
		OO::HANDLE maxa = fdm->readfds.max();
		OO::HANDLE maxb = fdm->writefds.max();
		OO::HANDLE maxc = fdm->errorfds.max();
		fdm->fdmax = ::max(maxa, ::max(maxb, maxc));
	}
}

void OThread::registerReadFD(OO::HANDLE fd, OIODevice* o) {
#ifndef NDEBUG
	cout<<"void OThread::registerReadFD(OO::HANDLE fd = \"" <<fd
			<<"\", OIODevice* o = \"" <<o <<"\" )" <<endl;
#endif
	
	this->allocThreadFd();
	
	//first make sure we are working with a valid file descriptor
	if(fd <= 0) return;
	
	//recalculate the max fd
	if(fd >= fdm->fdmax) fdm->fdmax = fd+1;
	
	//recalculate the min fd
	if(fd < fdm->fdmin) fdm->fdmin = fd;
	
	//set the fd
	fdm->readfds.set(fd, o);
}

void OThread::registerWriteFD(OO::HANDLE fd, OIODevice *o) {
#ifndef NDEBUG
	cout<<"void OThread::registerWriteFD(OO::HANDLE fd = \"" <<fd
			<<"\", OIODevice* o = \"" <<o <<"\" )" <<endl;
#endif
	
	this->allocThreadFd();
	
	//first make sure we are working with a valid file descriptor
	if(fd <= 0) return;
	
	//recalculate the max fd
	if(fd >= fdm->fdmax) fdm->fdmax = fd+1;
	
	//recalculate the min fd
	if(fd < fdm->fdmin) fdm->fdmin = fd;
	
	//set the fd
	fdm->writefds.set(fd, o);
}

void OThread::registerErrorFD(OO::HANDLE fd, OIODevice* o) {
#ifndef NDEBUG
	cout<<"void OThread::registerErrorFD(OO::HANDLE fd = \"" <<fd
			<<"\", OIODevice* o = \"" <<o <<"\" )" <<endl;
#endif
	
	this->allocThreadFd();
	
	//first make sure we are working with a valid file descriptor
	if(fd <= 0) return;
	
	//recalculate the max fd
	if(fd >= fdm->fdmax) fdm->fdmax = fd+1;
	
	//recalculate the min fd
	if(fd < fdm->fdmin) fdm->fdmin = fd;
	
	//set the fd
	fdm->errorfds.set(fd, o);
}

void OThread::unregisterReadFD(OO::HANDLE fd) {
#ifndef NDEBUG
	cout<<"void OThread::unregisterReadFD(OO::HANDLE fd = \"" <<fd
			<<"\" )" <<endl;
#endif
	
	this->allocThreadFd();
	
	//reset the maxfs or minfd values if they have changed
	recalcMinMax(fd);
	
	//clear the fd
	fdm->readfds.clear(fd);
}

void OThread::unregisterWriteFD(OO::HANDLE fd) {
#ifndef NDEBUG
	cout<<"void OThread::unregisterWriteFD(OO::HANDLE fd = \"" <<fd 
			<<"\" )" <<endl;
#endif
	
	this->allocThreadFd();
	
	//reset the maxfs or minfd values if they have changed
	recalcMinMax(fd);
	
	//clear the fd
	fdm->writefds.clear(fd);
}

void OThread::unregisterErrorFD(OO::HANDLE fd) {
#ifndef NDEBUG
	cout<<"void OThread::unregisterErrorFD(OO::HANDLE fd = \"" <<fd
			<<"\" )" <<endl;
#endif
	
	this->allocThreadFd();
	
	//reset the maxfs or minfd values if they have changed
	recalcMinMax(fd);
	
	//clear the fd
	fdm->errorfds.clear(fd);
}

void OThread::registerTimer(OTimerBase* tim) {
	this->allocThreadFd();
	
	//register this new timer in the delta list
	fdm->deltalist.push_back(TimerDelta(tim));
	
	//sort the list according to the timeout period
	/*
	sort(fdm->deltalist.begin(), fdm->deltalist.end(), 
		 [] (const TimerDelta& a, const TimerDelta& b) -> bool {
		return a.delta < b.delta;
	});
	*/
	sort(fdm->deltalist.begin(), fdm->deltalist.end(), deltaSort);
	
#ifdef __windows__
	fdm->tout = fdm->deltalist[0].timer->period().msec();
#else
	if(!fdm->tout.get()) fdm->tout.reset(new timeval);
	
	*fdm->tout = fdm->deltalist[0].timer->period().toTimeval();
#endif
}

void OThread::unregisterTimer(OTimerBase *tim) {
	this->allocThreadFd();
	
	for(unsigned i=0; i<fdm->deltalist.size(); i++) {
		if(fdm->deltalist[i].timer == tim) {
			fdm->deltalist.removeAt(i);
			break;
		}
	}
	
#ifdef __windows__
	if(fdm->deltalist.size() == 0) fdm->tout = INFINITE;
#else
	if(fdm->deltalist.size() == 0) fdm->tout.reset();
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

/*	Copyright (c) 2010 Michael F. Varga
 *	Email:		michael@engineerutopia.com
 *	Website:	engineerutopia.com
 *	
 *	Permission is hereby granted, free of charge, to any person obtaining a copy
 *	of this software and associated documentation files (the "Software"), to deal
 *	in the Software without restriction, including without limitation the rights
 *	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *	copies of the Software, and to permit persons to whom the Software is
 *	furnished to do so, subject to the following conditions:
 *	
 *	The above copyright notice and this permission notice shall be included in
 *	all copies or substantial portions of the Software.
 *	
 *	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 *	THE SOFTWARE.
*/

#ifndef OTHREAD_H
#define OTHREAD_H

#include<cstdlib>
#include<iostream>
#include<vector>
#include<functional>
#include<algorithm>
#include<memory>


#include<boost/bind.hpp>
#include<boost/thread/mutex.hpp>
#include<boost/interprocess/smart_ptr/unique_ptr.hpp>
#include<boost/scoped_ptr.hpp>
#include<auto_ptr.h>

namespace bst = boost;

namespace bip = boost::interprocess;

#include<OList.hpp>
#include<OIODevice.hpp>
#include<OTime.hpp>
#include<OTimerBase.hpp>
#include<OO.hpp>

#ifndef __windows__
	#include<pthread.h>
	#include<sys/select.h>
	#include<sys/signal.h>
	#include<sys/time.h>
#endif

#ifdef __apple__
#include<sys/sysctl.h>
#endif

using namespace std;
	
/// This struct stores a map of the OIODevice objects and
///	their associated file descriptors.
struct fdMap {
	fdMap() { fd = 0; obj = 0; }
	fdMap(OO::HANDLE f, OIODevice* o) { fd = f; obj = o; }
	~fdMap() { obj = 0; }
	
	OO::HANDLE fd;
	OIODevice* obj;
};

struct TimerDelta {
	TimerDelta(OTimerBase* b) {
		delta = b->period();
		timer = b;
	}
	
	void reset() {
		delta = timer->period();
	}
	
	bool operator==(OTimerBase* other) {
		return timer == other;
	}
	
	OTime delta;
	OTimerBase* timer;
};

class FdHandler {
public:
	FdHandler() {
		FD_ZERO(&fdset);
		devs.resize(FD_SETSIZE, 0);
	}
	
	FdHandler(const FdHandler& other) {
		devs = other.devs;
		
	}
	
	void set(OO::HANDLE h, OIODevice* d) {
		devs[h] = d;
		FD_SET(h, &fdset);
		
		if(h >= fdmax) fdmax = h + 1;
	}
	
	void clear(OO::HANDLE h) {
		if(unsigned(h) >= devs.size()) return;
		
		devs[h] = 0;
		FD_CLR(h, &fdset);
		
		if(h == fdmax-1) {
			for(int i=h; i>=0; i--) {
				if(devs[i]) {
					fdmax = i+1;
					return;
				}
			}
			fdmax = 0;
		}
	}
	
	OO::HANDLE min() const {
		if(devs.size()) {
			for(OO::HANDLE i=0; unsigned(i)<devs.size(); i++) {
				if(devs[i]) {
					return i;
				}
			}
		}
		
		return 0;
	}
	
	OIODevice* operator[](OO::HANDLE h) {
		return devs[h];
	}
	
	OO::HANDLE max() const {
		return fdmax;
	}
	
	bool isEmpty() const {
		return !devs.size();
	}
	
	int size() const {
		return devs.size();
	}
	
	fd_set& fdSet() {
		return fdset;
	}
	
protected:
	vector<OIODevice*> devs;
	
	OO::HANDLE fdmax;
	
#ifdef __windows__
//	vector<OO::HANDLE> fdset;
#else
	fd_set fdset;
#endif
};

struct OThreadFds {
	OThreadFds() : tout(NULL) {
		deltalist.clear();
		fdmin = 100000;
		fdmax = 0;
	}
	
	OThreadFds(const OThreadFds& other) : tout(NULL) {
		deltalist = other.deltalist;
		fdmin = other.fdmin;
		fdmax = other.fdmax;
		readfds = other.readfds;
		writefds = other.writefds;
		errorfds = other.errorfds;
	}
	
	OThreadFds& operator=(const OThreadFds& other) {
		deltalist = other.deltalist;
		tout.reset(NULL);
		
		return *this;
	}
	
	FdHandler readfds;
	FdHandler writefds;
	FdHandler errorfds;
	
	OO::HANDLE fdmax;
	
	OO::HANDLE fdmin;
	
	/// Delta list to store the list of active timers.
	OList<TimerDelta> deltalist;
	
	/// This is passed to the select call so it can return when
	/// a timer times out.
#ifdef __windows__
	DWORD tout;
#else
	auto_ptr<timeval> tout;
//	unique_ptr<timeval> tout;
#endif
	
};

/**	OThread is a simple wrapper around pOThread which adds some easy-to-use
 *	facilities for managing a large set of file descriptors. Preferably this
 *	class should be subclassed and all of the data managined inside that 
 *	class.
*/
class OThread {
public:
	friend class OTimer;
	friend class OSocket;
	friend class OPipe;
	friend class OSerial;
	
	/**	Create an empty OThread instance. Before this OThread can be started
	 *	a callback function must be set ( callback() ). Then the OThread may
	 *	be started with ( start() ).
	*/
	OThread();
	
	OThread(OThread& other);
	
	/**	Using this constructor is equavalent to calling the empty constructor
	 *	and then calling callback() and start().
	 *	\code
	 *	OThread* OThread = new OThread();
	 *	OThread->callback(bind(&SomeClass::ArbitraryFunction, AnInstance));
	 *	OThread->start();
	 *	\endcode
	*/
	OThread(function<void ()> cbk);
	
	/**	The destructor automatically haults the execution of the OThread 
	 *	even if it is actively running.
	*/
	~OThread();
	
	static OThread self();
	
	bool isSelf() const;
	
	bool operator==(OThread& thr) const;
	
	void cancel();
	
	/// Start the execution of the callback function by this OThread.
	void start();
	
	/// This is the function that is executed when the OThread is started.
	void callback(function<void ()> cbk);
	
	/// Return the number of processor on the local machine.
	int processors();
	
	/// Returns true if this OThread has been executed and has not returned.
	bool isRunning() const;
	
	///	This function starts the run loop to synchronize i/o operations on file
	/// descriptors and handle timers.
	void exec();
	
	bool execOnce();
	
	/// Clears all the buffers and registered file descriptors for this OThread 
	/// which will result in the termination of this OThread is the runloop is
	/// being used ( exec() function must be called ).
	void clear();
	
	enum ReadyStatus {
		Read,
		Write,
		Error
	};
	
	///	Children of this OThread call this to register their read fds
	///	so this OThread may do a blocked read.
	void registerReadFD(OO::HANDLE fd, OIODevice* o);
	
	///	Children of this OThread call this to register their write fds
	///	so this OThread may do a blocked write.
	void registerWriteFD(OO::HANDLE fd, OIODevice* o);
	
	///	Children of this OThread call this to register their error fds
	///	so this OThread may check for errors.
	void registerErrorFD(OO::HANDLE fd, OIODevice* o);
	
	///	Children of this OThread call this to unregister their read fds
	///	so they may be destroyed cleanly.
	void unregisterReadFD(OO::HANDLE fd);
	
	///	Children of this OThread call this to unregister their write fds
	///	so their may be destroyed cleanly.
	void unregisterWriteFD(OO::HANDLE fd);
	
	///	Children of this OThread call this to unregister their error fds
	///	so they may be destroyed cleanly.
	void unregisterErrorFD(OO::HANDLE fd);
	
	/// A child timer of this OThread calls this function to register
	/// itself.
	void registerTimer(OTimerBase* tim);
	
	/// A child timer of this OThread calls this function to unregister
	/// itself.
	void unregisterTimer(OTimerBase* tim);
	
protected:
	static bool deltaSort(const TimerDelta& a, const TimerDelta& b) {
		return a.delta < b.delta;
	}
	
	
	void recalcMinMax(OO::HANDLE fd);
	
	void allocThreadFd();
	
	auto_ptr<OThreadFds> fdm;
//	unique_ptr<OThreadFds> fdm;
	
	
	
	///	The OThread for this OThread object.
#ifdef __windows__
	HANDLE thread;
#else
	pthread_t thread;
#endif
	
	
	function<void ()> runCbk;
	void sigRun() {
		if(runCbk) {
			runCbk();
		} else {
			exec();
		}
	}
	
	///	This is where the processing actually starts.
#ifdef __windows__
	static DWORD WINAPI runThread(LPVOID arg);
#else
	static void* runThread(void* arg);
#endif
	
};

#endif // OThread_H

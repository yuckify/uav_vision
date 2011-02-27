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
	void registerPriorityFD(OO::HANDLE fd, OIODevice* o);
	
	///	Children of this OThread call this to unregister their read fds
	///	so they may be destroyed cleanly.
	void unregisterReadFD(OO::HANDLE fd);
	
	///	Children of this OThread call this to unregister their write fds
	///	so their may be destroyed cleanly.
	void unregisterWriteFD(OO::HANDLE fd);
	
	///	Children of this OThread call this to unregister their error fds
	///	so they may be destroyed cleanly.
	void unregisterPriorityFD(OO::HANDLE fd);
	
	/// A child timer of this OThread calls this function to register
	/// itself.
	void registerTimer(OTimerBase* tim);
	
	/// A child timer of this OThread calls this function to unregister
	/// itself.
	void unregisterTimer(OTimerBase* tim);
	
protected:
	
	/// This struct stores a map of the OIODevice objects and
	///	their associated file descriptors.
	struct fdMap {
		fdMap() { fd = 0; obj = 0; }
#ifdef __windows__
		fdMap(HANDLE f, OIODevice* o) { fd = f; obj = o; }
#else
		fdMap(int f, OIODevice* o) { fd = f; obj = o; }
#endif
		~fdMap() { obj = 0; }
		
#ifdef __windows__
		HANDLE fd;
#else
		int fd;
#endif
		OIODevice* obj;
	};
	
	int fdmax;
	
	bool is_running;
	
	///	Array of OIODevice map for reading.
	OList<fdMap> readMap;
	
	///	Array of OIODevice map for high priority data.
	OList<fdMap> priorityMap;
	
	/// The fd_set is needed for the select function to check for errors.
#ifdef __windows__
	OList<HANDLE> priorityfds;
#else
	fd_set priorityfds;
#endif
	
	///	The fd_set is needed for the select function.
#ifdef __windows__
	OList<HANDLE> readfds;
#else
	fd_set readfds;
#endif
	
	/// The fd_set is needed for the select function for the 
	/// write arguement.
#ifdef __windows__
	OList<HANDLE> writefds;
#else
	fd_set writefds;
#endif
	
	/// Array of OIODevice map for writing.
	OList<fdMap> writeMap;
	
	///	The OThread for this OThread object.
#ifdef __windows__
	HANDLE thread;
#else
	pthread_t thread;
#endif
	
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
	/// Delta list to store the list of active timers.
	OList<TimerDelta> deltalist;
	
	/// This is passed to the select call so it can return when
	/// a timer times out.
#ifdef __windows__
	DWORD tout;
#else
	unique_ptr<timeval> tout;
#endif
	
	function<void ()> runCbk;
	void sigRun() {
		is_running = true;
		if(runCbk) {
			runCbk();
		} else {
			exec();
		}
		is_running = false;
	}
	
	///	This is where the processing actually starts.
#ifdef __windows__
	static DWORD WINAPI runThread(LPVOID arg);
#else
	static void* runThread(void* arg);
#endif
	
};

#endif // OThread_H

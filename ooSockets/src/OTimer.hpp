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

#ifndef OTimer_H
#define OTimer_H

#include<OTime.hpp>
#include<OThread.hpp>
#include<OTimerBase.hpp>

/**	A simple OTimer class that calls a callback when
  *	the OTimer has expired. This OTimer is not intended for
  *	short precise timing values.
  *	
  *	A OTimer instance should be stored and parented 
  *	inside a OThread instance as follows:
  *	\code
  *	class SOThread : public OThread {
  *	public:
  *		SOThread() {
  *			OTimer = new OTimer(this);
  *			OTimer->callback(bind(&SOThread::timeFunction, this));
  *			OTimer->start(500, OO::Once);
  *		}
  *		
  *		void timeFunction() {
  *			cout<<"timeout" <<endl;
  *		}
  *		
  *	protected:
  *		OTimer* OTimer;
  *	}
  *	\endcode
 */
class OTimer : public OTimerBase {
public:
	/// Create a OTimer and parent it to a OThread.
	OTimer(OThread* parent = 0);
	
	/// Destructor.
	~OTimer();
	
	/// Get a pointer to the parent of this OTimer.
	OThread* parent();
	
	/// Set the parent of this OTimer to p.
	void parent(OThread* p);
	
	/// Start the OTimer.
	/// @param ms The period of time in milliseconds
	/// before the OTimer expires.
	void start(int ms, OO::TimerType t = OO::Once);
	
	/// Stop the OTimer.
	void stop();
	
	/// Get the timeout period of this OTimer.
	OTime period();
	
	/// Set the functor that will be called when
	/// this OTimer expires.
	void callback(function<void ()> cbk);
	
	/// Get the functor that will called.
	function<void ()> callback();
	
	/// The type of the OTimer. Returns either OO::Once 
	/// or OO::Repeat.
	OO::TimerType type();
	
protected:
	/// When the OTimer times out the OThread instance
	/// calls this. The OTimer instance then calls the
	/// callback.
	void runLoop();
	
	/// When this class is start this is called 
	/// to register the OTimer with the OThread parent.
	void registerTimer();
	
	/// When this class is destroyed this is called
	/// to unregister this instance from the parent
	/// OThread.
	void unregisterTimer();
	
	OThread* par;
	OTime tim;
	function<void ()> cbk;
	OO::TimerType ty;
};

#endif // OTimer_H

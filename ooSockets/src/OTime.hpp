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

#ifndef OTIME_H
#define OTIME_H

#include<math.h>
#include<sys/types.h>
#include<ostream>

#include<OO.hpp>

using namespace std;


/*
#if defined(_MSC_VER) || defined(__MINGW32__)
#  include <OTime.h>
#ifndef _OTimeVAL_DEFINED // also in winsock[2].h
#define _OTimeVAL_DEFINED
struct OTimeval {
    long tv_sec;
    long tv_usec;
};
#endif // _OTimeVAL_DEFINED
#else
#  include <sys/OTime.h>
#endif
*/

#if defined(_MSC_VER) || defined(__MINGW32__)
	#pragma comment(lib, "winmm.lib")
	static int gettimeofday(struct timeval* tp, void* tzp) {
	    DWORD t;
	    t = timeGetTime();
	    tp->tv_sec = t / 1000;
	    tp->tv_usec = (t % 1000)*1000;
	    // 0 indicates that the call succeeded.
	    return 0;
	}
#else
	#include<sys/time.h>
#endif

/**	This is a simple class that makes managing OTime
  *	a little easier.\n\n
  *	
  *	A simple example illustrating how to OTime the 
  *	duration of some work.\n
  *	\code
  *	OTime start = OTime::current();\n\n
  *	//
  *	//do some work\n\n
  *	//
  *	cout<<"Elapsed OTime: " <<OTime::current() - start <<endl;\n
  *	\endcode
 */
class OTime {
public:
	/// Initialized an empty OTime instance.
	OTime();
	
	/// Initialize this instance with some
	/// OTime value in milliseconds.
	OTime(int msec);
	
	/// Intialize this instance with a OTimeval
	/// structure.
	OTime(struct timeval t);
	
	/// Calculate the elapsed OTime in milliseconds
	/// and return it.
	int elapsed() const;
	
	/// Calculate the elapsed OTime in microseconds
	/// and return it.
	uint64_t uelapsed() const;
	
	/// Set the value of this OTime instance to the
	/// current OTime.
	void setCurrent();
	
	/// Get the current OTime.
	static OTime current();
	
	/// Return this instance as a OTimeval structure.
	timeval toTimeval();
	
	/// Return the OTime in seconds.
	int sec() const;
	
	/// Return the OTime in milliseconds.
	int64_t msec() const;
	
	/// Return the OTime in microseconds.
	int64_t usec() const;
	
	/// Set the OTime in seconds.
	void sec(int t);
	
	/// Set the OTime in milliseconds.
	void msec(int t);
	
	/// Set the OTime in microseconds.
	void usec(int64_t t);
	
	OTime operator-(OTime t);
	OTime operator+(OTime t);
	
	OTime& operator-=(const OTime& other);
	OTime& operator+=(const OTime& other);
	
	int compare(const OTime& t) const;
	
	bool operator<(const OTime& t) const;
	bool operator<=(const OTime& t) const;
	
	bool operator>(const OTime& t) const;
	bool operator>=(const OTime& t) const;
	
	bool operator==(const OTime& t) const;
	
private:
	int64_t us;
};

ostream& operator<<(ostream& out, const OTime& t);

#endif // OTime_H

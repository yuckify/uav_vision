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

#ifndef PIPE_H
#define PIPE_H

#include<stdlib.h>
#include<functional>

#include<OIODevice.hpp>
#include<OThread.hpp>
#include<OByteArray.hpp>
#include<OPipeError.hpp>

#ifndef __windows__
	#include<sys/ioctl.h>
	#include<unistd.h>
#endif

class OPipe : public OIODevice {
public:
	OPipe(OThread* parent = 0);
	OPipe(const OPipe& other, OThread* parent = 0);
	~OPipe();
	
	void operator=(const OPipe& other);
	
	int write(OByteArray& data, int len);
	int write(OByteArray& data);
	
	OByteArray readAll();
	OByteArray read(int len);
	
	void parent(OThread* p);
	
	int available();
	
	void readFunc(function<void ()> cbk);
	void errorFunc(function<void (OPipeError)> cbk);
	
protected:
	void readLoop();
	void writeLoop();
	void errorLoop();
	void priorityLoop();
	
	OO::HANDLE handle() const;
	
	OO::HANDLE readFd() const;
	void readFd(OO::HANDLE fd);
	OO::HANDLE writeFd() const;
	void writeFd(OO::HANDLE fd);
	
	void registerFd();
	void unregisterFd();
	
	///	File descriptor set. [0] = read, [1] = write\n
	OO::HANDLE fdes[2];
	
	///	This pointer is needed to register the read fd for synchronizing
	/// i/o for this among lots of other file descriptors.
	OThread* par;
	
	function<void ()> readCbk;
	void sigRead() {
		if(readCbk) {
			readCbk();
		}
	}
	
	function<void (OPipeError)> errorCbk;
	void sigError(OPipeError e = errno) {
		if(errorCbk) {
			errorCbk(e);
		}
	}
	
};

#endif // OPipe_H

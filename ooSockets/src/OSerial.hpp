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

#ifndef SERIAL_H
#define SERIAL_H

#include<errno.h>
#include<boost/regex.hpp>
#include<cstdlib>
#include<boost/filesystem.hpp>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/stat.h>


#include<OIODevice.hpp>
#include<OThread.hpp>
#include<OList.hpp>
#include<OO.hpp>
#include<OByteArray.hpp>
#include<OSerialError.hpp>

#ifndef __windows__
	#include<sys/ioctl.h>
	#include<termios.h>
	#include<sys/time.h>
	#include<unistd.h>
#endif

namespace fs = boost::filesystem;

/// This class contains some information on a single serial port.
/// If you want to get a list of the serial ports on your computer
/// call OSerial::portList().
class OPort {
	friend class OSerial;
public:
	OPort();
	
	/// On Mac OS and Windows the parameter type does nothing since
	/// these operating systems do not distinguish between hardware
	/// and usb serial ports.
	OPort(unsigned int index, OO::SerialType type);
	
	/// This is the system independant name of the port.
	/// The naming convention is "OSerial[0-9]+" (Windows, Mac, Linux).
	/// Linux distinguises between usb serial adapters and hw serial
	/// ports. On Linux "OSerial[0-9]+" is used for hardware serial
	/// ports and "UsbOSerial[0-9]+" is used for usb to serial
	/// adapters.
	OString name() const;
	
	/// This is the system dependant name of the serial which is
	/// used internally to open the serial port.
	OString hwName() const;
	
	/// This parameter is used only on Linux to distinguish between
	/// hardware and usb serial ports.
	OO::SerialType type() const;
	
	/// This returns true if type() == OO::NullOPort. OO::NullOPort
	/// specifies that this OPort class was not initialized by
	/// OSerial.
	bool isEmpty() const;
	
protected:
	OString				p_device;
	OString				p_name;
	int					p_index;
	OO::SerialType		p_type;
};

template <class T = OPort> class basic_portlist : public OList<OPort> {
public:
	basic_portlist<> filterByType(OO::SerialType type, OO::MatchType match = OO::And) {
		basic_portlist<> ret;
		
		for(auto it=this->begin(); it<this->end(); it++) {
			if(it->type() == type && match == OO::And) {
				ret.push_back(*it);
			} else if(it->type() != type && match == OO::Not) {
				ret.push_back(*it);
			}
		}
		
		return ret;
	}
	
};

typedef basic_portlist<> OPortList;

class OSerial : public OIODevice {
public:
    OSerial(OThread* parent = 0);
	~OSerial();
	
	/**	Get a list of the available serial ports on the computer.
	 */
	static OPortList portList();
	
	/**	Open a serial port.
	  *	@param speed The speed of the serial port being opened.
	  *	@param port The port to be opened.
	  *	@param opts Options to open the port with.
	  *	@see OSerial::error() If an error is generated check this 
	  *		for the error code and strError() for the string 
	  *		representation.
	 */
	void open(OO::SerialSpeed	speed	= OO::O9600, 
			  const OPort&		port	= OPort(), 
			  int				opts	= OO::DefaultOpts);
	
	/**	Read data from the serial port.
	  *	@param data This is where the data will be stored.
	  *		All previous data in this container will be cleared.
	  *	@param len The amount of data to be read.
	  *	@see OSerial::error() If an error is generated check this 
	  *		for the error code and strError() for the string 
	  *		representation.
	 */
	OByteArray read(int len);
	
	/**	Read data from the serial port.
	  *	@param data All available data will be stored here.
	  *		All previous data in this container will be cleared.
	  *	@see OSerial::error() If an error is generated check this 
	  *		for the error code and strError() for the string 
	  *		representation.
	 */
	OByteArray readAll();
	
	/**	Write everything in data to the serial port.
	  *	@param data The data to be written to the port.
	 */
	void write(OByteArray& data);
	
	/// Get the parent Thread for this i/o device.
	OThread* parent();
	
	/// Set the parent Thread for this i/o device.
	void parent(OThread* parent);
	
	/// Get the numberical error code.
	int error();
	
	/// Get the string representation of the error code.
	OString strError();
	
	///	Set the callback that will be called when data is available.
	void readyReadFunc(function<void ()> cbk);
	
	/// Set the callback that will be called when an error is generated.
	void errorFunc(function<void (OSerialError)> cbk);
	
	/// Get the amount of data available on the port.
	int available();
	
	/**	Wait for data to be available on the port.
	  *	@param msec The amount of time to wait for data to
	  *		be available. When data becomes available the
	  *		readyRead callback will be called.
	 */
	void waitForReadyRead(int msec);
	
	OSerial& operator=(OSerial& other);
	
	
protected:
	OO::HANDLE fileDescriptor() const;
	
	/**	This should be called when it has already been determined
	  *	that there is an error on the file desciptor that needs
	  *	to be handled. The error callback will be called if 
	  *	there is an error that needs to be handled.
	 */
	void errorLoop();
	
	/**	This function is not used. It is here just because the
	  *	IODevice superclass requires it to exist.
	 */
	void writeLoop();
	
	/**	This should be called when it has already been determined
	  *	that there is data on the port to be read. The callback
	  *	readyRead will be called if there is data to be handled.
	 */
	void readLoop();
	
	void priorityLoop();
	
	//register the file descriptor with the parent thread
	//object if it is not already registered
	void registerFD();
	
	//unregister the file descriptor with the parent thread
	//object, normally this happens on destruction
	void unregisterFD();
	
	
	//this callback is called when an error is generated
	function<void (OSerialError)> errorCbk;
#ifdef __windows__
	void sigError(int e = GetLastError()) {
#else
	void sigError(int e = errno) {
#endif
		serialerr = e;
		if(errorCbk) errorCbk(e);
	}
	
	//this callback is called when data is available
	function<void ()> readyReadCbk;
	void sigReadyRead() {
		if(readyReadCbk) readyReadCbk();
	}
	
	
	//the parent thread for this i/o object
	OThread* par;
	
	//store the last error code generated
	OSerialError serialerr;
	
	//the file descriptor that points to the file
	OO::HANDLE fdes;
	
	bool fdreg;
	
};

#endif // OSerial_H

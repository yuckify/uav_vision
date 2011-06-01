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

#ifndef SOCKET_H
#define SOCKET_H

#include<string.h>
#include<stdlib.h>
#include<functional>


#include<OString.hpp>
#include<OByteArray.hpp>
#include<OList.hpp>
#include<OIODevice.hpp>
#include<OThread.hpp>
#include<OSockAddress.hpp>
#include<OAddressInfo.hpp>
#include<OSockError.hpp>
#include<OO.hpp>
#include<OMacAddress.hpp>
#include<ONet.hpp>

#ifdef OO_QT
#include<QObject>
#include<QSocketNotifier>
#endif

/*
#ifdef __linux__
	#define GS_NOSIGNAL MSG_NOSIGNAL
	
	//AF_LINK (BSD) == PF_PACKET (linux)
	#ifndef AF_LINK
		#define AF_LINK PF_PACKET
	#endif
	
#elif defined(__apple__)
	#include<pcap/bpf.h>
	#include<net/if_dl.h>
	#define GS_NOSIGNAL SO_NOSIGPIPE
	
#endif
*/

//windows unix compatibility
#ifdef __windows__
	
	#define SHUT_RDWR SD_BOTH
	
	#define _error WSAGetLastError()
	#define _clearError() (WSAGetLastError())
	#define _close(x) closesocket(x)
	#define _ioctl(a, b, c) ioctlsocket(a, b, c)
#else
	
	#include<sys/types.h>
	#include<sys/socket.h>
	#include<sys/ioctl.h>
	#include<netinet/in.h>
	#include<arpa/inet.h>
	#include<netdb.h>
	#include<errno.h>
	#include<signal.h>
	#include<sys/wait.h>
	#include<fcntl.h>
	#include<ifaddrs.h>
	#include<tr1/functional>
	#include<unistd.h>
	
	#define _error errno
	#define _clearError() (errno = 0)
	#define _close(x) ::close(x)
	#define _ioctl(a, b, c) ::ioctl(a, b, c)

#endif

//using namespace std::placeholders;

/*!	This is a simple socket class that provides an easy to use
 *	interface for BSD sockets. The OThread object is typically used
 *	since it contains a run loop that enables the callbacks in this
 *	class. In order for the OThread object to call the callbacks in
 *	OSocket the OThread::exec() function must be called.
*/
class OSocket : public OIODevice {
#ifdef OO_QT
	Q_OBJECT
#endif
public:
	/**	Constructor.
	 *	
	 *	@param parent The parent of this OSocket. The OThread object
	 *		contains a run loop that enables the callbacks in this
	 *		socket object to work.
	*/
	OSocket(OThread* parent = 0);
	
#ifdef OO_QT
	OSocket(QObject* parent);
#endif
	
	OSocket(const OSocket& other);
	
	~OSocket();
	
	/// Get socket information about a host.
	static OAddressList getAddressInfo(OString			address, 
									   unsigned short	port, 
									   OO::SockType	type	= OO::NullType, 
									   OO::SockFamily	family	= OO::NullFamily, 
									   OO::SockFlags	flags	= OO::NullFlag,
									   int*				errval	= 0);
	
	/// Get socket information about a host.
	static OAddressList getAddressInfo(OString		address, 
									   OString		service, 
									   OAddressInfo	hints	= OAddressInfo(),
									   int*			errval	= 0);
	
	
	/// Get the number of incomming connections that will be stored on listen.
	int backlog();
	
	///	Close the connection.
	void close();
	
	/// Read in 'len' bytes of data and place it in 'ba'.
//	virtual OByteArray read(int len) = 0;
	
	/// Read in all the data from the socket and place it in 'data'.
//	virtual OByteArray readAll() = 0;
	
	/// Write the entire contents of 'data' to the socket.
//	virtual int write(OByteArray& data) = 0;
	
	/// Check if the ready write signal has been enabled.
	bool readyWriteEnabled() const;
	
	/// When the ready write signal is enabled the signal will be emitted
	/// when the socket is ready to receive more data. When bulk data 
	/// needs to be transfered this signal should be enabled.
	void enableReadyWrite();
	
	/// Disable the ready write signal. If this signal is still enabled
	/// and no bulk data is being transfered, the signal will automatically
	/// disabled if no data has been written since the last call to the 
	/// signal.
	void disableReadyWrite();
	
	/// Get the sockaddress of the connected peer (Tcp) or the address
	/// a packet was received from (Udp).
	OSockAddress peerAddress();
	
	///	Get the hostname of the local machine.
	static OString hostName();
	
	/// Set the hostname of the local machine.
	static bool hostName(OString name);
	
	/// Get the most recently generated error.
	int error() const;
	
	/// Get the string representation of the most recently
	/// generated error.
	OString strerror() const;
	
	///	Initialize connect callback.
	void connectFunc(function<void ()> cbk);
	
	///	Initialize disconnect callback.
	void disconnectFunc(function<void ()> cbk);
	
	///	Initialize error callback. Called when an error is
	/// generated on the socket.
	void errorFunc(function<void (OSockError)> cbk);
	
	/// Initialize high priority data callback. When a packet of
	/// high priority data is received this callback is called.
	void highPriorityFunc(function<void ()> cbk);
	
	/// Initialize ready write callback. Called when the 
	/// network stack is ready to handle more data.
	void readyWriteFunc(function<void ()> cbk);
	
	///	Initialize ready read callback. Called when there
	/// is data available to be read on the socket.
	void readyReadFunc(function<void ()> cbk);
	
	/// Called when incomming connection is pending.
	void incommingFunc(function<void (int)> cbk);
	
	/// Call this with a properly initialized GTcpServer socket
	/// to wait for an incomming tcp connection.
	bool waitForIncommingConnection(int msec = 30000);
	
	/// Call this with a properly initialized GTcpOSocket or
	/// GUdpOSocket to wait for incomming data.
	bool waitForReadyRead(int msec = 30000);
	
	/// Call this with a properly initialize GTcpOSocket or
	/// GUdpOSocket to wait until the next call to write()
	/// can be made.
	bool waitForReadyWrite(int msec = 30000);
	
	/// Set the amount of received data the network stack is to
	/// buffer.
	void setRecvBufferSize(int size = 500000);
	
	/// Set the amount of sent data the network stack is to
	/// buffer.
	void setSendBufferSize(int size = 500000);
	
	
	int recvBufferSize();
	
	int sendBufferSize();
	
	void setBroadcast(bool b);
	bool broadcast();
	
	///	Returns the amount of data available in the receive buffer.
	int available();
	int bytesAvailable();
	
	/// Get the BSD file descriptor for this socket.
	OO::HANDLE handle() const;
	
	///	Set the BSD file descriptor for this socket.
	void setHandle(OO::HANDLE des);
	
	void operator=(OO::HANDLE h);
	
	/// Get the parent thread for this socket.
	OThread* parent();
	
	/// Set the parent thread for this socket.
	void setParent(OThread* p);
	
	
	//TODO
	//make this do something
	bool isEmpty();
	
	/// Generate a magick packet to be broadcasted for Wake on LAN.
	static OByteArray magicPacket(OMacAddress mac);
	
	/// This loop is only to be called after a call to select()
	/// has establish that there is an impending error on the 
	/// underlying socket. If there is an impending error this loop
	/// calls the error callback to report it.
	virtual void errorLoop();
	
	/// This loop is only to be called after a call to select()
	/// has established that data is available to be read from
	/// the unerlying socket. If there is data to be read this
	/// loop calls the readyRead callback.
	virtual void readLoop();
	
	/// This loop is only to be called after a call to select()
	/// has established that more data can be written to the
	/// underlying socket. If more data can be written to the
	/// socket the readyRead callback is called. If no calls
	/// to write() were made inside the callback then this 
	/// callback will be disabled so it does not get called
	/// continuously even if nothing is happening.
	virtual void writeLoop();
	
	/// This loop is only to be called after a call to select()
	/// has established that an incomming connection needs to
	/// be handled. The incommingConnection callback is called
	/// when an incomming connection may be handled.
	virtual void incommingLoop();
	
	/// This loop is only to be called after a call to select()
	/// 
	virtual void priorityLoop();
	
#ifdef OO_QT
public slots:
	virtual void readyReadSlot(int socket);
	virtual void readyWriteSlot(int socket);
#endif
protected:
	///	Open a connection to someone. By default the connection is Tcp and 
	/// either IPv4 or IPv6.
	bool connect(OString			addr, 
				 unsigned short		port, 
				 OO::SockType		type, 
				 OO::SockFamily	family	= OO::NullFamily);
	
	/// Open a connection to someone with a given OSockAddress.
	bool connect(const OSockAddress&	addr,
				 OO::SockType			type);
	
	/// Open a connection to someone with a given OAddressInfo class.
	bool connect(const OAddressInfo& info);
	
	/// Setup to wait for someone to open a connection.
	bool listen(const OAddressInfo& info);
	
	bool listen(const OSockAddress&	sock, 
				OO::SockType		type);
	
	/// Set to wait for someone to open a connection.
	bool listen(int				port, 
				OString			ifn		= "", 
				OO::SockType	type	= OO::Tcp, 
				OO::SockFamily	family	= OO::NullFamily);
	
	
	unsigned writestat;
	unsigned esigstat;
	
	
	int written;
	
	///	The file descriptor for the connection.
	OO::SOCKET fdes;
	
	/// The most recently generated socket error.
	OSockError sockerr;
	
	///	This parent thread of this object. Use this pointer to
	///	register fds so the thread may do a blocking read.
	OThread* par;
	
#ifdef OO_QT
	/// This is the parent of this object, for QT compatibility.
	QObject* qtpar;
	
	unique_ptr<QSocketNotifier> qt_read;
	unique_ptr<QSocketNotifier> qt_write;
	
	/// This is kind of a hack... This is set by subclass OTcpServer
	/// so when the QSocketNotifier sends a signal that there is
	/// activity on the socket. The OTcpServer instance can call
	/// incommingLoop(). Incomming connections are the only activity
	/// a listening tcp socket will ever experience.
	function<void ()> readyReadPatch;
	
#endif
	
	OSockAddress recvaddr;
	OSockAddress sendaddr;
	
	/// This gets called when it comes time to register the file
	/// descriptor with the parent thread for the run loop.
	void registerFD() {
		if(par && fdes) {
			par->registerReadFD((OO::HANDLE)fdes, this);
		}
#ifdef OO_QT
		if(qtpar && fdes) {
			qt_read.reset(new QSocketNotifier(fdes, 
											  QSocketNotifier::Read, this));
			QObject::connect(qt_read.get(), SIGNAL(activated(int)), 
							 this, SLOT(readyReadSlot(int)));
		}
#endif
	}
	
	/// This get called when it comes time to unregister the file
	/// descriptors with the parent, when shutting down.
	void unregisterFD() {
		if(par && fdes) {
			par->unregisterReadFD((OO::HANDLE)fdes);
			par->unregisterWriteFD((OO::HANDLE)fdes);
			par->unregisterErrorFD((OO::HANDLE)fdes);
		}
#ifdef OO_QT
		if(qtpar) {
			qt_read.reset(NULL);
			qt_write.reset(NULL);
		}
#endif
	}
	
	static void* get_in_addr(struct sockaddr* sa);
	
	function<void ()> highPriorityCbk;
	void sigHighPriority() {
		if(highPriorityCbk) highPriorityCbk();
	}
	
	/// Callback, called when the socked it ready for writing.
	function<void ()> readyWriteCbk;
	void sigReadyWrite() {
		if(readyWriteCbk) readyWriteCbk();
	}
	
	///	Callback, called when the connection is opened.
	function<void ()> connectCbk;
	void sigConnect() {
		if(connectCbk) connectCbk();
	}
	
	///	Callback, called when the connection is closed.
	function<void ()> disconnectCbk;
	void sigDisconnect() {
		close();
		if(disconnectCbk) disconnectCbk();
	}
	
	///	Callback, called when an error is generated.
	function<void (OSockError)> errorCbk;
	void sigError(int e = _error) {
		sockerr = e;
		if(errorCbk) errorCbk(sockerr);
	}
	
	///	Callback, called when data is available in the buffer.
	function<void ()> readyReadCbk;
	void sigReadyRead() {
		if(readyReadCbk)
			readyReadCbk();
	}
	
	/// Callback, called when an incomming connection is pending
	/// on a tcp listening socket.
	function<void (int)> incommingCbk;
	void sigIncomming(int fd) {
		if(incommingCbk) incommingCbk(fd);
	}
	
#ifndef __windows__
	static void sigchld_handler(int s);
#endif
	
	function<void (OSockError)> tmp;
	
	/// to enable the error signal when we want it to begin emitting the
	/// error signal
	void enableErrorSig() {
		if(!esigstat) {
			tmp = errorCbk;
			errorCbk = NULL;
			esigstat = true;
		}
	}
	
	/// to diable the error signal when we want it to stop emitting
	/// for example when a function is being called internally but
	/// we don't want it to emit any error signals
	void disableErrorSig() {
		if(esigstat) {
			errorCbk = tmp;
			esigstat = false;
		}
	}

	static bool initSockLimit;

};

#endif // SOCKET_H

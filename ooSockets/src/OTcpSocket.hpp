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

#ifndef OTcpSocket_H
#define OTcpSocket_H

#include"OSocket.hpp"

/**	OTcpSocket is a simple wrapper around GSocket that provides a simple interface
 *	for managing a connection socket.
 *	\code
 *	class SThread : public Thread {
 *	public:
 *		SThread() {
 *			sock = new OTcpSocket(this);
 *			sock->disconnectFunc(bind(&SThread::disconnected, this));
 *			sock->errorFunc(bind(&SThread::error, this _1));
 *			sock->readyReadFunc(bind(&SThread::readyRead, this));
 *			sock->connect("localhost", 12345);
 *		}
 *		
 *		void readyRead() {
 *			cout<<"Data Available: " <<sock->available() <<endl;
 *		}
 *		
 *		void disconnected() {
 *			cout<<"Disconnected" <<endl;
 *		}
 *		
 *		void error(GSockError e) {
 *			cout<<"Error: " <<e <<endl;
 *		}
 *		
 *	protected:
 *		OTcpSocket* sock;
 *	}
 *	\endcode
*/
class OTcpSocket : public OSocket {
public:
	/**	A GTcpServer instance must be parented to a GThread instance to 
	 *	utilize the callbacks set with calls to readyReadFunc(), errorFunc()
	 *	or disconnectFunc().
	*/
	OTcpSocket(OThread* parent = 0);
	
#ifdef OO_QT
	OTcpSocket(QObject* parent);
#endif
	
	/**	Open a connection to a host.
	 *	@param addr The string address of the host to connect to.
	 *	@param port The port on which to connect.
	 *	@param family The address family to use.
	*/
	bool connect(OString			addr, 
				 unsigned short		port, 
				 OO::SockFamily	family = OO::NullFamily);
	
	/**	Open a connection to a host.
	 *	@param addr The address of the host to connect to.
	*/
	bool connect(const OSockAddress& addr);
	
	/**	Open a connection to a host.
	 *	@param info The information of the host to connect to.
	*/
	bool connect(const OAddressInfo& info);
	
	/**	Read in a set amount of data and append it to the data
	 *	container passed as an input parameter.
	*/
	int read(OByteArray& data, int len);
	
	/**	Read all the data available on the socket and append
	 *	it to the end of data container passed as an input
	 *	parameter.
	*/
	int readAll(OByteArray& data);
	
	/**	Read some data from the socket.
	 *	@param data The container to store the binary data in.
	 *	@param len The amount of data to read in bytes.
	*/
	OByteArray read(int len);
	
	/**	Read all the available data from the socket.
	 *	@param data The container to store the binary data in.
	*/
	OByteArray readAll();
	
	/**	Write all the available data to the socket.
	 *	@param data The data to be written.
	*/
	int write(OByteArray &data);
};

#endif // OTcpSocket_H

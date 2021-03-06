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

#ifndef OTcpServer_H
#define OTcpServer_H

#include"OSocket.hpp"

/**	
 *	
 *	
*/
class OTcpServer : public OSocket {
public:
	/**	This OTcpServer instance must be parented to a GThread instance to 
	 *	utilize the callbacks set with calls to readyReadFunc(), errorFunc()
	 *	or disconnectFunc().
	*/
	OTcpServer(OThread* parent = 0);
	
#ifdef OO_QT
	OTcpServer(QObject* parent);
#endif
	
	/// Setup to wait for someone to open a connection.
	bool listen(const OAddressInfo& info);
	
	/// Setup to wait for someone to open a connection.
	bool listen(const OSockAddress&	sock);
	
	/// Set to wait for someone to open a connection.
	bool listen(int				port, 
				OString			ifn		= "", 
				OO::SockFamily	family	= OO::NullFamily);
	
	OByteArray read(int len);
	OByteArray readAll();
	
	int write(OByteArray &data);
	
	void readLoop();
	void writeLoop();
	
#ifdef OO_QT
public slots:
	void readyReadSlot();
	
#endif
	
};

#endif // OTcpServer_H

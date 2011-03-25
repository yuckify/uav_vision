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

#ifndef ODSMUX_H
#define ODSMUX_H

#include<deque>
#include<memory>
#include<list>
#include<vector>
#include<algorithm>
#include<mutex>

using namespace std;

#include<OByteArray.hpp>
#include<OTcpSocket.hpp>
#include<OThread.hpp>
#include<OTcpServer.hpp>
#include<OString.hpp>
#include<ODataStream.hpp>

/*
class ODataStream<>;
class ODSQueue;
class ODSMux;
*/

/*
class ODSMux {
public:
	ODSMux(OThread* parent);
	ODSMux(OThread* parent, OTcpSocket* socket);
	ODSMux(OThread* parent, int fd);
	
	ODataStream connect(OString addr, unsigned short port);
	
	ODataStream connect(const OSockAddress& addr);
	
	ODataStream connect(const OAddressInfo& info);
	
	bool listen(int port, OString ifn = "");
	
	bool listen(const OAddressInfo& info);
	
	bool listen(const OSockAddress&	sock);
	
	int size() const;
	ODataStream operator[](int i);
	
protected:
	OThread*				q_parent;
	OTcpServer				q_serv;
	vector<ODataStream>	q_ques;
	
	//TODO
	//when host connects with unique identifier, use predefined template
	//to initialize the queue callbacks
};
*/

#endif // ODSMUX_H

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

#ifndef OUdpSocket_H
#define OUdpSocket_H

#include<OSocket.hpp>
#include<OThread.hpp>
#include<OSockAddress.hpp>
#include<OO.hpp>

#ifndef __windows__
	#include<netinet/in.h>
#endif

/**	OUdpSocket is a simple wrapper around GSocket that provides a simple interface
 *	for managing a connectionless socket. This class also provides functions
 *	for multicast, broadcast and sending a magic packet for Wake on LAN.
 *	\code
 *	class SThread : public OThread {
 *	public:
 *		SThread() {
 *			sock = new OUdpSocket(this);
 *			sock->errorFunc(bind(&SThread::error, this _1));
 *			sock->readyReadFunc(bind(&SThread::readyRead, this));
 *			sock->connect("localhost", 12345);
 *		}
 *		
 *		void readyRead() {
 *			cout<<"Data Available: " <<sock->available() <<endl;
 *		}
 *		
 *		void error(GSockError e) {
 *			cout<<"Error: " <<e <<endl;
 *		}
 *		
 *	protected:
 *		OUdpSocket* sock;
 *	}
 *	\endcode
*/
class OUdpSocket : public OSocket {
public:
	/**	A GUdpServer instance must be parented to a OThread instance to 
	 *	utilize the callbacks set with calls to readyReadFunc(), errorFunc()
	 *	or disconnectFunc().
	*/
	OUdpSocket(OThread* parent = 0);
	
	/**	Listen for udp packets.
	 *	@param port The port to listen for packets on.
	 *	@param addr The ip address of the interface to listen on.
	 *	@return If an error occured bind() will return false.
	*/
	bool bind(int				port, 
			  OString			addr	=	"", 
			  OO::SockFamily	family	=	OO::NullFamily);
	
	/**	Listen for udp packets.
	 *	@param addr The socket address to listen for incomming udp packets on.
	 *	The address class must at least contain a valid port number to bind to.
	 *	@return If an error occured bind() will return false.
	*/
	bool bind(OSockAddress addr);
	
	/**	Listen for udp packets.
	 *	@param info The socket address to listen for incomming udp packets on.
	 *	The info class must at least contain a valid port number to bind to.
	 *	@return If an error occured bind() will return false.
	*/
	bool bind(OAddressInfo info);
	
	/**	Performs the necesary name resolution and stores the resulting
	 *	OSockAddress internally for repeated use.
	 *	@param addr The string address of the host.
	 *	@param port The port the packet will be sent to.
	 *	@param family The address family ( IPv4 or IPv6 ) of the socket.
	*/
	void sendTo(unsigned short		port, 
				OString				addr, 
				OO::SockFamily		family = OO::NullFamily);
	
	/**	Performs the necesary name resolution and stores the resulting
	 *	OSockAddress internally for repeated use.
	 *	@param addr The address of the host to send packets to.
	*/
	void sendTo(OSockAddress addr);
	
	/**	Performs the necesary name resolution and stores the resulting
	 *	OSockAddress internally for repeated use.
	 *	@param addr The address of the host to send packets to.
	*/
	void sendTo(OAddressInfo addr);
	
	/**	Send packets to a multicast group.
	 *	@param group the groupd to send the packets to.
	 *	@param port The port number.
	*/
	void sendMulticast(unsigned short port, OString group);
	
	/**	Send packets to a muticast group with address addr.
	*/
	void sendMulticast(const OSockAddress& addr);
	
	/**	Listen for packets coming from a multicast group.
	 *	@param group The ip string of the multicast group.
	 *	@param port The port the packet is send to.
	 *	@param ifa The network interface to listen on.
	*/
	bool listenMulticast(unsigned short port, 
						 OString group,
						 OString ifa = "");
	
	/**	
	 *	
	*/
	bool listenMulticast(unsigned short	port, 
						 OString		group,
						 ONet			ifa);
	
	bool listenMulticast(OSockAddress group, 
						 ONet ifa);
	
	bool listenMulticast(OSockAddress group,
						 OSockAddress ifa = OSockAddress());
	
	bool addMembership(OSockAddress	group, 
					   OSockAddress		ifa		= OSockAddress());
	
	bool addMembership(OString group, 
					   OString ifa		= "");
	
	/**	Set the time to live of the packets this socket sends.
	*/
	void setTTL(unsigned int len);
	
	/**	Get the time to live for the socket.
	*/
	unsigned int getTTL();
	
	/**	Set weather or not this socket will send broadcast packets.
	 *	@param s If true this socket will send broadcast packets.
	 *	@return True if this parameter could be set.
	*/
	bool broadcast(bool s);
	
	/**	Gets wether or not this socket was set to transmit broadcast packets.
	*/
	bool broadcast();
	
	/**	Transmits a Wake on LAN magic packet.
	 *	@param broad The broadcast address to send to.
	 *	@param mac The mac address of the receiving computer.
	*/
	static void txMagicPacket(OSockAddress& broad, OMacAddress& mac);
	
	/**	Read some data from the socket.
	 *	@param ba The container to store the binary data in.
	 *	@param len The amount of data to read in bytes.
	*/
	OByteArray read(int len);
	
	/**	Read some data from the socket.
	 *	@param addr The address the data was sent from.
	 *	@param ba The container to store the binary data in.
	 *	@param len The amount of data to read in bytes.
	*/
	OByteArray read(OSockAddress& addr, int len);
	
	/**	Read all the available data on the socket for the pending packet.
	 *	@param The container to store the binary data in.
	*/
	OByteArray readAll();
	
	/**	Read all the available data on the socket for the pending packet.
	 *	@param addr The address the data was sent from.
	 *	@param The container to store the binary data in.
	*/
	OByteArray readAll(OSockAddress& addr);
	
	/**	Write all the available data to the socket. The data will be sent to
	 *	the internally stored address from the sendTo() call.
	 *	@param data The data to be written.
	*/
	int write(const OByteArray &data);
	
	/**	Write all the available data to the socket.
	 *	@param addr The address the data will be sent to.
	 *	@param data The data to be written.
	*/
	int write(const OSockAddress &addr, const OByteArray& data);
};

#endif // OUdpSocket_H

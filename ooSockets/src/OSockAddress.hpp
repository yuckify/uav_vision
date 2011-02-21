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

#ifndef SOCKADDRESS_H
#define SOCKADDRESS_H

#include<cstring>

#include<OString.hpp>
#include<OO.hpp>

#ifndef __windows__
	#include<netinet/in.h>
	#include<arpa/inet.h>
	#include<netdb.h>
#endif

/*!	This class acts as a simple wrapper around the bsd structures sockaddr, 
 *	sockaddr_in, sockaddr_in6 and related functions to set and retreive data. 
 *	These bsd structures tell a socket where to point to when creating a new 
 *	socket. The most important elements to set when initializing a OSockAddress 
 *	are family, type, port and ip address.
*/
class OSockAddress {
	friend class OUdpSocket;
public:
	OSockAddress();
	
	/// Create a OSockAddress from a string ip. "scope" and "flow" only apply to ipv6.
	OSockAddress(OString ip, unsigned short port, unsigned scope = 0, unsigned flow = 0);
	
	/// Create a socket address of address family f.
	/// f could be OO::Inet4, OO::Inet6, OO::Unix, or OO::Any.
	OSockAddress(OO::SockFamily f);
	
	/// Destructor
	~OSockAddress();
	
	/// Get a pointer to the bsd socket sockaddr structure.
	sockaddr* data();
	
	const sockaddr* constData() const;
	
	/// Get the size of the bsd socket sockaddr structure.
	int size() const;
	
	void clear();
	
	/// Set a OSockAddress with a string ip. "scope" and "flow" only apply to ipv6.
	void set(OString ip, unsigned short port, unsigned scope = 0, unsigned flow = 0);
	
	/// Set the socket address using a bsd sockaddr structure.
	void set(const sockaddr& in);
	
	/// Set the socket address using a bsd sockaddr_in structure.
	void set(const sockaddr_in& in);
	
	/// Set the socket address using a bsd sockaddr_in6 structure.
	void set(const sockaddr_in6& in);
	
	/// Calls set(sockaddr&).
	void operator=(const sockaddr* in);
	
	/// Calls set(sockaddr&).
	void operator=(const sockaddr& in);
	
	/// Calls set(sockaddr_in&).
	void operator=(const sockaddr_in& in);
	
	/// Calls set(sockaddr_in6&).
	void operator=(const sockaddr_in6& in);
	
	/// Calls set(sockaddr_in&).
	void operator=(const sockaddr_in* in);
	
	/// Calls set(sockaddr_in6&).
	void operator=(const sockaddr_in6* in);
	
	void operator=(const OSockAddress& in);
	
	/// Checks to see if this class is pointing to
	/// any sockaddr structure.
	bool isEmpty() const;
	
	/// Get the family in the sockaddr structure.
	OO::SockFamily family() const;
	
	/// Set the family in the sockaddr structure.
	void family(OO::SockFamily f);
	
	/// Checks if the sockaddr structure is not empty and
	/// is of socktye Inet4.
	bool isIpv4() const;
	
	/// Checks if the sockaddr structure is not empty and
	/// is of socktype Inet6.
	bool isIpv6() const;
	
	/// Get the port in the sockaddr structure.
	int port() const;
	
	/// Set the port in the sockaddr structure.
	void port(unsigned short p);
	
	
	/// Get the ip address in the sockaddr structure.
	OString ipString() const;
	
	/// Set the ip address in the sockaddr structure.
	void ipString(OString a);
	
private:
	sockaddr_storage addr;
	
};
	
#endif // OSockAddress_H

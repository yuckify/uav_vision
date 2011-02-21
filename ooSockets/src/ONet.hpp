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

#ifndef NET_H
#define NET_H

#include<memory>
#include<sstream>
#include<boost/regex.hpp>

#include<OSockAddress.hpp>
#include<OList.hpp>
#include<OMacAddress.hpp>
#include<OO.hpp>

#ifndef __windows__
	#include<sys/socket.h>
	#include<net/if.h>
#endif

/*!	The ONetList is what the GSocket class returns when a list of networking 
 *	interfaces is requested. This class acts to wrap a single network interface 
 *	in a easy to use manner.
*/
class ONet {
	friend class OSocket;
public:
	ONet();
	
	/// Name of the interface.
	OString name() const;
	
	/// Set the name of the interface.
	void name(OString str);
	
	/// Mac address of the interface.
	OString macString() const;
	
	/// Get the mac in this list.
	OMacAddress mac() const;
	
	/// Set the mac for this interface.
	void mac(const OMacAddress& data);
	
	/// Mac data for the interface.
	const char* constMacData() const;
	
	/// Set the ip4 or ip6 address depending on the family of the input.
	void ipAddress(const OSockAddress& ip);
	
	/// Get the string representation of the ip4 address if it is set else 
	/// return the string of the ip6 address else return a null string.
	OString ipString() const;
	
	/// Get the ip4 sockaddress if not available get the ip6 sockaddress 
	/// else return any empty sockaddress.
	OSockAddress ipAddress() const;
	
	/// Ipv4 address of the interface.
	OString ip4String() const;
	
	/// Set the Ipv4 address.
	void ip4Address(const OSockAddress& ip);
	
	const OSockAddress& ip4Address() const;
	
	OSockAddress& ip4Address();
	
	/// Ipv6 address of the interface
	OString ip6String() const;
	
	/// Set the Ipv6 address.
	void ip6Address(const OSockAddress& ip);
	
	/// Get the ip6 sockaddress.
	const OSockAddress& ip6Address() const;
	
	OSockAddress& ip6Address();
	
	/// Ipv4 broadcast address.
	OString ipBroadcast() const;
	
	/// Set the broadcast ip address.
	void ipBroadcast(const OSockAddress& ip);
	
	/// Set the subnetmask.
	void subnetMask(const OSockAddress& mask);
	
	/// Get the subnetmask.
	OSockAddress subnetMask();
	
	/// Check if there is any data in this net class.
	bool isEmpty() const;
	
	/// This is a system specific index. This is used to specify the
	/// interface when setting up an ipv6 multicast listening socket.
	unsigned index() const;
	
protected:
	OString			ifa_name;		//network interface name
	OMacAddress		ifa_mac;		//mac address
	OSockAddress	ifa_addr;		//ipv4 address
	OSockAddress	ifa_addr6;		//ipv6 address
	OSockAddress	ifa_addrbroad;	//broadcast address
	OSockAddress	ifa_netmask;	//subnet mask
};

/**	This list template was implemented specifically for the ONet class. It adds a 
 *	few features to make it easier to manage a list of network interfaces.
*/
template <class T = ONet> class basic_netlist : public OList<ONet> {
public:
	/**	Filter the list of network interfaces based on some regular expression. 
	 *	This function uses boost::regex to match.
	 *	@param str The regular expression to be matched.
	 *	@return The list of network interfaces where the name matches the regular 
	 *	expression.
	*/
	basic_netlist<T> filterByName(OString str) {
		boost::regex exp(str);
		
		basic_netlist<T> ret;
		
		unsigned length = size();
		for(unsigned i=0; i<length; i++) {
			if(boost::regex_match(at(i).name(), exp))
				ret.push_back(at(i));
		}
		
		return ret;
	}
	
	basic_netlist<T> filterByIp4(OString str) {
		boost::regex exp(str);
		
		basic_netlist<T> ret;
		
		unsigned length = size();
		for(unsigned i=0; i<length; i++) {
			if(boost::regex_match(at(i).ip4String(), exp))
				ret.push_back(at(i));
		}
		
		return ret;
	}
	
	basic_netlist<T> filterByIp6(OString str) {
		boost::regex exp(str);
		
		basic_netlist<T> ret;
		
		unsigned length = size();
		for(unsigned i=0; i<length; i++) {
			if(boost::regex_match(at(i).ip6String(), exp))
				ret.push_back(at(i));
		}
		
		return ret;
	}
	
};

typedef basic_netlist<> ONetList;

#endif // ONet_H

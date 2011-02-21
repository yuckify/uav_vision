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

#ifndef OADDRESSINFO_H
#define OADDRESSINFO_H

#include<OSockAddress.hpp>
#include<OList.hpp>
#include<OO.hpp>

#ifndef __windows__
	#include<netdb.h>
#endif

/**	This is a simple wrapper around the BSD socket api
  *	addrinfo structure to query the location of a hostname.
  *	This class is used with a call to GSocket::getOAddressInfo().
  *	
 */
class OAddressInfo {
public:
	/// Create an empty instance.
	OAddressInfo();
	
	/// Initialize an instance from an addrinfo structure.
	OAddressInfo(const addrinfo* other);
	
	/// Copy a addrinfo structure.
	void operator=(const addrinfo* other);
	
	/// Get the current flags.
	OO::SockFlags flags() const;
	
	/// Set the flags overwritting all the old settings.
	void flags(OO::SockFlags f);
	
	/// Add a new flags to the list.
	void appendFlags(OO::SockFlags f);
	
	/// Get the address family of the socket.
	OO::SockFamily family() const;
	
	/// Set the address family of the socket.
	void family(OO::SockFamily f);
	
	/// Get the the type of the socket.
	OO::SockType socktype() const;
	
	///	Set the type of the socket.
	void socktype(OO::SockType t);
	
	/// Get the protocol for the socket.
	OO::SockProto protocol() const;
	
	/// Set the protocol for the socket.
	void protocol(OO::SockProto p);
	
	/// Get a reference to the address for the socket.
	OSockAddress address() const;
	
	/// Set the address for the socket.
	void address(const OSockAddress& addr);
	
	/// Get the hostname.
	OString name() const;
	
	/// Set the hostname.
	void name(OString str);
	
	/// Check if this instance contains any valid data.
	bool isEmpty() const;
	
private:
	OO::SockFlags	ai_flags;
	OO::SockFamily	ai_family;
	OO::SockType	ai_socktype;
	OO::SockProto	ai_protocol;
	OSockAddress	ai_addr;
	OString			ai_canonname;
};

/*!	This list template was implemented specifically for the OAddressInfo
 *	class. It adds a few features to make it easier to manage a 
 *	list of addresses.
*/
template <typename T = OAddressInfo> 
					   class basic_addresslist : 
					   public OList<OAddressInfo> {
public:
	
	/*!	\brief Filter list of socket addresses by type.
	 *	
	 *	Filter the list of addresses based on the type. The resultant list can 
	 *	be an inclusive or exculsive subset of the main list and the filter option.
	 *	@param type The filter parameter the socket type is being compared to.
	 *	@param match The type of logic performed when matching the list of 
	 *	addresses by the comparator, inclusive or exclusive.
	 *	@return The list of addresses where the type field matches or does not 
	 *	match depending on the match option.
	*/
	basic_addresslist<T> filterByType(OO::SockType	type, 
									   OO::MatchType	match = OO::And) {
		basic_addresslist<T> ret;
		
		unsigned length = size();
		for(unsigned i=0; i<length; i++) {
			if(at(i).socktype() == type && match == OO::And)
				ret.push_back(at(i));
			else if(at(i).socktype() != type && match == OO::Not)
				ret.push_back(at(i));
		}
		
		return ret;
	}
	
	/*!	\brief Filter list of socket addresses by family.
	 *	
	 *	Filter the list of addresses based on the family. The resultant  list can 
	 *	be an inclusive or exculsive subset of the main list and the filter option.
	 *	@param family The filter parameter the socket family is being compared to.
	 *	@param match The type of logic performed when matching the list of 
	 *	addresses by the comparator, inclusive or exclusive.
	 *	@return The list of addresses where the family field matches or does not 
	 *	match depending on the match option.
	*/
	basic_addresslist<T> filterByFamily(OO::SockFamily	family,
										 OO::MatchType		match = OO::And) {
		basic_addresslist<T> ret;
		
		unsigned length = size();
		for(unsigned i=0; i<length; i++) {
			if(at(i).family() == family && match == OO::And)
				ret.push_back(at(i));
			else if(at(i).family() != family && match == OO::Not)
				ret.push_back(at(i));
		}
		
		return ret;
	}
	
	
};

typedef basic_addresslist<> OAddressList;

#endif // OAddressInfo_H

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

#ifndef MACADDRESS_H
#define MACADDRESS_H

#include<cstring>
#include<sstream>

#include<OString.hpp>
#include<OByteArray.hpp>

using namespace std;

#define _GMAC_SIZE 6

/**	The OMacAddress class acts as a simple container for the mac address of a network 
 *	interface. 
*/
class OMacAddress {
public:
	/// Create an empty container.
	OMacAddress();
	
	/// Initialize with a pointer to a region of memory
	/// containing the mac address.
	OMacAddress(const void* dat);
	
	/// Initialize with 
	OMacAddress(OString s);
	
	/// Get the string representation of the mac address, 
	/// in the format aa:bb:cc:dd:ee:ff.
	OString str() const;
	
	/// Set the mac address with a string of the format
	/// aa:bb:cc:dd:ee:ff.
	bool str(OString s);
	
	/// Check to see if the mac container has anything in it.
	bool isEmpty() const;
	
	/// Get a pointer to the mac data.
	char* data();
	
	/// Get a const pointer to the mac data.
	const char* constData() const;
	
	/// Get the size of the memory region containing the mac;
	unsigned size() const;
	
	/// Compare if this mac is equal to another.
	bool operator==(OMacAddress&& other);
	
protected:
	/// The MAC.
	char m[8];
	
};

OByteArray& operator<<(OByteArray& data, OMacAddress& addr);
OByteArray& operator>>(OByteArray& data, OMacAddress& addr);

#endif // OMacAddress_H

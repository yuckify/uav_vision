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

#ifndef OVersion_H
#define OVersion_H

#include"OString.hpp"

class OVersion {
public:
	OVersion(OString str = "");
	
	void operator=(OString str);
	void set(OString str);
	
	bool operator>(const OVersion& ver) const;
	bool operator>=(const OVersion& ver) const;
	bool operator<(const OVersion& ver) const;
	bool operator<=(const OVersion& ver) const;
	bool operator==(const OVersion& ver) const;
	
	int compare(const OVersion& ver) const;
	
	unsigned major();
	unsigned mid();
	unsigned minor();
	
protected:
	unsigned a;
	unsigned b;
	unsigned c;
};

#endif // OOVersion_H

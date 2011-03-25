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

#ifndef STRING_H
#define STRING_H

#if defined(QT_CORE_LIB)
	#include<QString>
	#define _O_QSTRING_SUPPORT_
#endif

#include<string.h>
#include<istream>
#include<ostream>
#include<time.h>
#include<stdlib.h>
#include<cstddef>


#include<OList.hpp>
#include<OO.hpp>
#include<OTime.hpp>

#ifndef __windows__
	#include<inttypes.h>
	#include<sys/time.h>
#endif

using namespace std;

/*!	This string class was writen mainly for the purpose of
 *	implementing enhanced number to text conversion and text
 *	to number conversion and contains a few functions the 
 *	std::string class badly needs.
*/
class OString : public string {
public:
	/*!	The default constructor. Initializes the OString with no
	 *	memory.
	*/
	OString();
	
	/*!	Initialize with a std::string.
	 *	
	 *	Example:\n
	 *	string str1 = "test";\n
	 *	OString str2(str1);		//str2 = "test"
	 *	
	 *	@param str The string to initialize this OString with.
	 */
	OString(string str);
	
	/*!	Initialize with a c string.
	 *	
	 *	Example:\n
	 *	const char* str1 = "test";
	 *	OString str2(str1);			//str2 = "test"
	 *	
	 *	@param str The string to initialize this OString with.
	*/
	OString(const char* str);
	
	/*!	Initialize with a c string and specify the number of byte to use.
	 *	The OString will be initialize with len number of characters or
	 *	the length of the c string, which ever is smaller.
	 *	
	 *	@param str The string to initialize this OString with.
	 *	@param len The number of characters to use from the
	 *	initializing string.
	*/
	explicit OString(const char* str, int len);
	
	/*!	This constructor initializes this OString with another
	 *	OString.
	 *	
	 *	@param str The string to initialize this OString with.
	*/
	OString(const OString& str);
	
#ifdef _O_QSTRING_SUPPORT_
	/*!	Initialize with a QOString. Added for QT support.
	 *	
	 *	Example:\n
	 *	QOString str1 = "test";\n
	 *	OString str2(str1);	//str2 = "test"
	 *	
	 *	@param str The QOString to initialize this OString with.
	*/
	OString(QString str);
#endif
	
	
	/*!	Set this OString equal to the string representation of
	 *	a number;
	 *	
	 *	Example:\n
	 *	short test = 123;\n
	 *	OString str;\n
	 *	str = test;	//str = "123"
	 *	
	 *	@param i The number to be converted to text;
	*/
	OString& operator=(short i);
	
	/*!	Set this OString equal to the string representation of
	 *	a number;
	 *	
	 *	Example:\n
	 *	unsigned short test = 123;\n
	 *	OString str;\n
	 *	str = test;	//str = "123"
	 *	
	 *	@param i The number to be converted to text;
	*/
	OString& operator=(unsigned short i);
	
	/*!	Set this OString equal to the string representation of
	 *	a number;
	 *	
	 *	Example:\n
	 *	int test = 123;\n
	 *	OString str;\n
	 *	str = test;	//str = "123"
	 *	
	 *	@param i The number to be converted to text;
	*/
	OString& operator=(int i);
	
	/*!	Set this OString equal to the string representation of
	 *	a number;
	 *	
	 *	Example:\n
	 *	unsigned int test = 123;\n
	 *	OString str;\n
	 *	str = test;	//str = "123"
	 *	
	 *	@param i The number to be converted to text;
	*/
	OString& operator=(unsigned int i);
	
	/*!	Set this OString equal to the string representation of
	 *	a number;
	 *	
	 *	Example:\n
	 *	long test = 123;\n
	 *	OString str;\n
	 *	str = test;	//str = "123"
	 *	
	 *	@param i The number to be converted to text;
	*/
	OString& operator=(long i);
	
	/*!	Set this OString equal to the string representation of
	 *	a number;
	 *	
	 *	Example:\n
	 *	unsigned long test = 123;\n
	 *	OString str;\n
	 *	str = test;	//str = "123"
	 *	
	 *	@param i The number to be converted to text;
	*/
	OString& operator=(unsigned long i);
	
	/*!	Set this OString equal to the string representation of
	 *	a number;
	 *	
	 *	Example:\n
	 *	double test = 123.123;\n
	 *	OString str;\n
	 *	str = test;	//str = "123.123"
	 *	
	 *	@param i The number to be converted to text;
	*/
	OString& operator=(double i);
	
	/*!	Set this OString equal to the string representation of
	 *	a number;
	 *	
	 *	Example:\n
	 *	float test = 123.123;\n
	 *	OString str;\n
	 *	str = test;	//str = "123.123"
	 *	
	 *	@param i The number to be converted to text;
	*/
	OString& operator=(float i);
	
	/*!	Stream data into this OString object.
	 *	
	 *	Example:\n
	 *	OString str1;\n
	 *	OString str2 = "test";\n
	 *	OString str3 = "\n";\n
	 *	str1<<str2 <<str3;	//str1 = "test\n"
	 *	
	 *	@param str The string to be streamed into this object.
	*/
	OString& operator<<(OString str);
	
	/*!	Stream data into this OString object.
	 *	
	 *	Example:\n
	 *	OString str1;\n
	 *	string str2 = "test";\n
	 *	string str3 = "\n";\n
	 *	str1<<str2 <<str3;	//str1 = "test\n"
	 *	
	 *	@param str The string to be streamed into this object.
	*/
	OString& operator<<(string& str);
	
	/*!	Stream data into this OString object.
	 *	
	 *	Example:\n
	 *	OString str1;\n
	 *	const char* str2 = "test";\n
	 *	const char* str3 = "\n";\n
	 *	str1<<str2 <<str3;	//str1 = "test\n"
	 *	
	 *	@param str The string to be streamed into this object.
	*/
	OString& operator<<(const char* str);
	
	OString& operator<<(const char ch);
	
	OString& operator<<(bool i);
	
	/*!	Stream data into this OString object.
	 *	
	 *	Example:\n
	 *	OString str;\n
	 *	short a = 123;\n
	 *	str<<test;	//str = "123"
	 *	
	 *	@param i The number to be streamed into this object.
	*/
	OString& operator<<(short i);
	
	/*!	Stream data into this OString object.
	 *	
	 *	Example:\n
	 *	OString str;\n
	 *	unsigned short a = 123;\n
	 *	str<<test;	//str = "123"
	 *	
	 *	@param i The number to be streamed into this object.
	*/
	OString& operator<<(unsigned short i);
	
	/*!	Stream data into this OString object.
	 *	
	 *	Example:\n
	 *	OString str;\n
	 *	int a = 123;\n
	 *	str<<test;	//str = "123"
	 *	
	 *	@param i The number to be streamed into this object.
	*/
	OString& operator<<(int i);
	
	/*!	Stream data into this OString object.
	 *	
	 *	Example:\n
	 *	OString str;\n
	 *	unsigned int a = 123;\n
	 *	str<<test;	//str = "123"
	 *	
	 *	@param i The number to be streamed into this object.
	*/
	OString& operator<<(unsigned int i);
	
	/*!	Stream data into this OString object.
	 *	
	 *	Example:\n
	 *	OString str;\n
	 *	long a = 123;\n
	 *	str<<test;	//str = "123"
	 *	
	 *	@param i The number to be streamed into this object.
	*/
	OString& operator<<(long i);
	
	/*!	Stream data into this OString object.
	 *	
	 *	Example:\n
	 *	OString str;\n
	 *	unsigned int a = 123;\n
	 *	str<<test;	//str = "123"
	 *	
	 *	@param i The number to be streamed into this object.
	*/
	OString& operator<<(unsigned long i);
	
	/*!	Stream data into this OString object.
	 *	
	 *	Example:\n
	 *	OString str;\n
	 *	float a = 123.123;\n
	 *	str<<test;	//str = "123.123"
	 *	
	 *	@param i The number to be streamed into this object.
	*/
	OString& operator<<(float i);
	
	/*!	Stream data into this OString object.
	 *	
	 *	Example:\n
	 *	OString str;\n
	 *	double a = 123.123;\n
	 *	str<<test;	//str = "123.123"
	 *	
	 *	@param i The number to be streamed into this object.
	*/
	OString& operator<<(double i);
	
	/*!	Sets all the characters in the OString to lower case.
	 *	
	 *	Example:\n
	 *	OString str = "AbCdEfGh";\n
	 *	str.toLower();	//str = "abcdefgh"
	 */
	OString& toLower();
	
	/*!	Sets all the characters in the OString to upper case.
	 *	
	 *	Example:\n
	 *	OString str = "AbCdEfGh";\n
	 *	str.toUpper();	//str = "ABCDEFGH"
	*/
	OString& toUpper();
	
	/*!	sdbm hash of this string.
	 *	
	*/
	uint64_t hashSDBM() const;
	
	uint64_t hashAP() const;
	
	uint64_t hashRS() const;
	
	uint64_t hash() const;
	
#if defined(_O_QSTRING_SUPPORT_)
	QString toQString() const;
#endif
	
	/*!	Returns true if this OString is empty.
	 *	
	 *	@return A bool specified true if this OString is empty
	 *	else false.
	*/
	bool isEmpty() const;
	
	/*!
	 *
	*/
	void pop_back();
	
	/*!
	 *
	*/
	void pop_front();
	
	/*!
	 *
	*/
	void appendFront(const string& str);
	
	/*!
	 *
	*/
	void appendFront(const char ch);
	
	/*!
	 *
	 *
	*/
	void append(const OString& str);
	
	/*!	Append a boolean value to the end of the string.
	 *	
	 *	Example:\n
	 *	OString str = "test: ";\n
	 *	str.append(true);	str = "test: 1"\n
	 *	
	 *	@param i The boolean value to append to the end of the string.
	 *	@return This OString.
	*/
	void append(bool i);
	
	/*!	Append a short value to the end of the string.
	 *	
	 *	Example:\n
	 *	OString str = "test: ";\n
	 *	short num = 123;\n
	 *	str.append(num);	str = "test: 123"\n
	 *	
	 *	@param i The short value to append to the end of the string.
	 *	@return This OString.
	*/
	void append(short i);
	
	/*!	Append a unsigned short value to the end of the string.
	 *	
	 *	Example:\n
	 *	OString str = "test: ";\n
	 *	unsigned short num = 123;\n
	 *	str.append(num);	str = "test: 123"\n
	 *	
	 *	@param i The unsigned short value to append to the end of the string.
	 *	@return This OString.
	*/
	void append(unsigned short i);
	
	/*!	Append a short value to the end of the string.
	 *	
	 *	Example:\n
	 *	OString str = "test: ";\n
	 *	int num = 123;\n
	 *	str.append(num);	str = "test: 123"\n
	 *	
	 *	@param i The int value to append to the end of the string.
	 *	@return This OString.
	*/
	void append(int i);
	
	/*!	Append a unsigned int value to the end of the string.
	 *	
	 *	Example:\n
	 *	OString str = "test: ";\n
	 *	unsigned int num = 123;\n
	 *	str.append(num);	str = "test: 123"\n
	 *	
	 *	@param i The unsigned int value to append to the end of the string.
	 *	@return This OString.
	*/
	void append(unsigned int i);
	void append(long i);
	void append(unsigned long i);
	void append(float i);
	void append(double i);
	
	/*!
	 *
	*/
	const char* toCString() const;
	
	/*!	
	*/
	OString operator+(const OString str);
	
	/*!	
	*/
	OString operator+(const char* str);
	
	/*!	
	*/
	OString operator+(const string& str);
	
	/*!	
	*/
	OString operator+(const char c);
	
	OString operator+(bool i);
	OString operator+(short i);
	OString operator+(unsigned short i);
	OString operator+(int i);
	OString operator+(unsigned int i);
	OString operator+(long i);
	OString operator+(unsigned long i);
	OString operator+(float i);
	OString operator+(double i);
	
	/*!	Generate a substring from start index to end index.
	 *	
	 *	@param start The beginning index of the substring to
	 *	be generated.
	 *	@param end The ending index of the substring to be
	 *	generated.
	 *	@return The generated substring.
	*/
	OString substring(int start, int end = 0) const;
	
	/*!	Generate a substring before the first occurance of str.
	 *	
	 *	OString str = "Foo Bar Baz";
	 *	OString str2 = str.substringBefore(" Bar");		//str2 = "Foo"
	 *	
	 *	@param str An expression that is search.
	 *	@return The generate substring.
	*/
	OString substringBefore(const OString str) const;
	OString substringBefore(const char* str) const;
	
	OString mid(int i, int len = 0) const;
	
	OString substringAfter(const OString str) const;
	OString substringAfter(const char* str) const;
	
	OList<OString> split(const char* exp) const;
	OList<OString> split(const OString& exp) const;
	OList<OString> split(const string& exp) const;
	OList<OString> split(const char c) const;
	
	//******************************
	//to search in a OString
	//******************************
	int indexOf(const char in, int start = 0) const;
	int indexOf(const OString str, int start = 0) const;
	int indexOf(const char* str, int start = 0) const;
	
	bool contains(const char* str) const;
	bool contains(const OString& str) const;
	
	int instancesOf(const char* str) const;
	int instancesOf(const OString str) const;
	int instancesOf(const char c) const;
	
	//******************************
	//comparision
	//******************************
	bool startsWith(const char* str) const;
	bool startsWith(const OString str) const;
	
	bool endsWith(const OString str) const;
	
	bool operator==(const OString& str) const;
	bool operator==(const char* str) const;
	bool operator==(const string& str) const;
	bool operator!=(const OString& str) const;
	bool operator!=(const char* str) const;
	bool operator>=(const OString str) const;
	bool operator>=(const string& str) const;
	bool operator>=(const char* str) const;
	bool operator>(const OString str) const;
	bool operator>(const char* str) const;
	bool operator>(const string& str) const;
	bool operator<=(const OString str) const;
	bool operator<=(const char* str) const;
	bool operator<=(const string& str) const;
	bool operator<(const OString str) const;
	bool operator<(const char* str) const;
	bool operator<(const string& str) const;
	
	//******************************
	//to convert a string into a number
	//******************************
	
	int toInt(bool* ok = 0, int base = 10) const;
	unsigned int toUInt(bool* ok = 0, int base = 10) const;
	long toLong(bool* ok = 0, int base = 10) const;
	unsigned long toULong(bool* ok = 0, int base = 10) const;
	double toDouble(bool* ok = 0) const;
	
	template<class T> T as() const {
		T select;
		return to(select);
	}
	
	//******************************
	//to convert a number into a string
	//******************************
	static OString number(long num, int base = 10);
	static OString number(long unsigned int num, int base = 10);
	static OString number(int num, int base = 10);
	static OString number(unsigned int num, int base = 10);
	static OString number(short num, int base = 10);
	static OString number(unsigned short num, int base = 10);
	static OString number(double num, int precision = 6);
	
	//******************************
	//some manual memory management
	//******************************
	void reserve(int size);
	
	static OString random(int len);
	
protected:
#ifdef __windows__
	int to(int unused) const { return toInt(); }
	unsigned int to(unsigned int unused) const { return toUInt(); }
	long to(long unused) const { return toLong(); }
	unsigned long to(unsigned long unused) const { return toULong(); }
	double to(double unused) const { return toDouble(); }
	string to(string unused) const { return *this; }
	OString to(OString unused) const { return *this; }
#else	
	int to(int unused __attribute__((unused))) const { return toInt(); }
	unsigned int to(unsigned int unused __attribute__((unused))) const { return toUInt(); }
	long to(long unused __attribute__((unused))) const { return toLong(); }
	unsigned long to(unsigned long unused __attribute__((unused))) const { return toULong(); }
	double to(double unused __attribute__((unused))) const { return toDouble(); }
	string to(string unused __attribute__((unused))) const { return *this; }
	OString to(OString unused __attribute__((unused))) const { return *this; }
#endif
	
	/*!	All number to text conversion for unsigned numbers
	 *	are a derivative of this function.
	 *	
	 *	@param num The number to be converted to its textual
	 *	representation.
	*/
	static OString convertNum(long num, int base);
	
	/*!	All number to text conversion for signed numbers
	 *	are a derivative of this function.
	 *	
	 *	@param num The number to be converted to its textual
	 *	representation.
	*/
	static OString convertNum(unsigned long num, int base);
	
	static int orderOfMagnitude(unsigned long num, int base);
	static int orderOfMagnitude(long num, int base);
	
	static int decode(int ch);
	
	static int encode(int num);
	
	static bool isValid(int ch, int base);
	
};//end OString

std::size_t hash_value(OString const& arg);

typedef OList<OString> OStringList;

OString& operator+(const char c, OString& str);
OString& operator+(const string& str1, OString& str2);
OString& operator+(const char* str1, OString& str2);

ostream& operator<<(ostream& os, const OString& str);
istream& operator>>(istream& is, OString& str);

#endif // OString_H

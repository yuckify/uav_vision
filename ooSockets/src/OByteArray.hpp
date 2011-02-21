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

#ifndef OBYTEARRAY_H
#define OBYTEARRAY_H

#include<stdarg.h>
#include<string.h>
#include<stdint.h>
#include<memory>
#include<iostream>
#include<typeinfo>
#include<algorithm>
#include<map>
#include<fstream>
#include<functional>

#include<boost/crc.hpp>

#include<OString.hpp>

#ifndef __windows__
	#include<tr1/functional>
	using namespace std::placeholders;
#endif

using namespace std;

class OByteArray;

/**	This is a simple class that allows the OByteArray class to intrusively 
 *	serialize any arbitrary object.
*/
class OSerializable {
	friend class OByteArray;
	
protected:
	virtual void serialize(OByteArray& ar, OO::ArrayBase ba) = 0;
	
};



/**	This is a simple class that provides and easy to use
 *	interface for packing and extracting binary data in an array. 
 *	A good example of how this class might be used is to pack 
 *	data into the array so it may be transmitter over a serial
 *	port or socket.\n
 *	Basic Usage:
 *	\code
 *	//here is all our data to be packet into a binary array
 *	OString str = "Hello World";
 *	uint16_t number = 123;
 *	int16_t anothernumber = -2345;
 *	uint8_t header = 0x05;
 *	uint16_t length = 0;
 *	
 *	//instantiate the OByteArray and pack the data
 *	//the length variable will act as a placeholder
 *	//we will insert a value for it later after we know how
 *	//long the OByteArray will actually be
 *	OByteArray pack;
 *	pack<<length <<header <<str <<number <<anothernumber;
 *	
 *	//tell the write pointer to go back to the beginning
 *	//so we can write in our value for 'length'
 *	pack.seek(0);
 *	length = pack.size();
 *	pack<<length;
 *	
 *	//here we have some arbitrary socket or serial interface
 *	//that was previously setup
 *	sock->write(pack);
 *	\endcode
*/
class OByteArray {
public:
	/**	Create a OByteArray with a default length of 10 or
	 *	n bytes;
	 *	@param n The length of the array to start with.
	*/
	OByteArray(int n = 10);
	
	/**	The copy constructor.
	 *	@param old The OByteArray obect being copied.
	*/
	OByteArray(const OByteArray& old);
	
	/**	Initialize a OByteArray with an arbitrary block of memory.
	 *	@param data The block to data to initialize with.
	 *	@param len The amount of data to be used in bytes.
	*/
	OByteArray(const char* data, int len);
	OByteArray(const char* str);
	OByteArray(const OString& str);
	~OByteArray();
	
	template <class T> OByteArray& operator&(T& item) {
		OByteArray& arr = *this;
		if(mem->dir == OO::Input) {
			arr<<item;
		} else {
			arr>>item;
		}
		return *this;
	}
	
	void append(OByteArray& ba);
	void append(OByteArray&& ba);
	void append(const OString& str);
	void append(const OString&& str);
	void append(const string str);
	void append(const char* str);
	void append(const char* str, int len);
	void append(OSerializable& obj);
	
	void append(bool i);
	void append(int8_t i);
	void append(int16_t i);
	void append(int32_t i);
	void append(int64_t i);
	void append(uint8_t i);
	void append(uint16_t i);
	void append(uint32_t i);
	void append(uint64_t i);
	
	template <class T> void append(const vector<T>& vec) {
		OByteArray& arr = *this;
		uint32_t length = vec.size();
		arr<<length;
		for(unsigned int i=0; i<length; i++)
			arr<<(T&)vec[i];
	}
	
	template <class T, class U> void append(const map<T, U>& ma) {
		OByteArray& arr = *this;
		uint32_t length = ma.size();
		arr<<length;
		
		for_each(ma.begin(), ma.end(), [&arr] (const std::pair<T, U>& pa) {
			arr<<(T&)pa.first <<(U&)pa.second;
		});
	}
	
	OByteArray& operator<<(bool i);
	OByteArray& operator<<(int8_t i);
	OByteArray& operator<<(int16_t i);
	OByteArray& operator<<(int32_t i);
	OByteArray& operator<<(int64_t i);
	OByteArray& operator<<(uint8_t i);
	OByteArray& operator<<(uint16_t i);
	OByteArray& operator<<(uint32_t i);
	OByteArray& operator<<(uint64_t i);
	
	
	OByteArray& operator<<(OSerializable& obj);
	OByteArray& operator<<(const char* str);
	OByteArray& operator<<(string& str);
	OByteArray& operator<<(OString& str);
	OByteArray& operator<<(OString&& str);
	OByteArray& operator<<(OByteArray& data);
	
	template <class T> OByteArray& operator<<(const vector<T>& vec) {
		this->append(vec);
		return *this;
	}
	
	template <class T, class U> OByteArray& operator<<(const map<T, U>& ma) {
		this->append(ma);
		return *this;
	}
	
	OByteArray& operator>>(bool i);
	OByteArray& operator>>(uint8_t& i);
	OByteArray& operator>>(uint16_t& i);
	OByteArray& operator>>(uint32_t& i);
	OByteArray& operator>>(uint64_t& i);
	OByteArray& operator>>(int8_t& i);
	OByteArray& operator>>(int16_t& i);
	OByteArray& operator>>(int32_t& i);
	OByteArray& operator>>(int64_t& i);
	
	
	OByteArray& operator>>(OSerializable& obj);
	OByteArray& operator>>(OString& str);
	
	template <class T> OByteArray& operator>>(vector<T>& vec) {
		this->read(vec);
		return *this;
	}
	
	template <class T, class U> OByteArray& operator>>(map<T, U>& ma) {
		this->read(ma);
		return *this;
	}
	
	template <class T> void read(vector<T>& vec) {
		uint32_t len = 0;
		OByteArray& arr = *this;
		arr>>len;
		
		for(uint32_t i=0; i<len; i++) {
			T xfer;
			arr>>xfer;
			vec.push_back(xfer);
		}
	}
	
	template <class T, class U> void read(map<T, U>& ma) {
		OByteArray& arr = *this;
		uint32_t length;
		arr>>length;
		
		for(uint32_t i=0; i<length; i++) {
			T key;
			U value;
			
			arr>>key >>value;
			
			ma[key] = value;
		}
	}
	
	/**	Load the binary data into this data container.
	 *	@param fn The name of the file to load.
	*/
	bool loadFile(OString fn);
	
	/**	Save the binary data in this data container to a file.
	 *	@param fn The name of the file to save the data to.
	*/
	bool saveFile(OString fn);
	
	/**	This OByteArray is split into an array of pieces of
	 *	a specified size.
	 *	@param size The size of the pieces that we are making.
	*/
	OList<OByteArray> chunk(int size);
	
	/**	This OByteArray is split into an array of pieces of
	 *	a specified size. A functor may be passes as the
	 *	second parameter. This functor must take 3 argurments.
	 *	The first arguement is the OByteArray for the chunk 
	 *	we are about to create. The second arguement is the
	 *	current index of the OByteArray for the chunk we are
	 *	about to create. The third arguement is the size of 
	 *	OByteArray for the chunk we are about to create.
	 *	@param size The size of the pieces that we are making.
	 *	@param cbk The functor to be called so data may be packed
	 *	to to the beginning of the OByteArray before the chunk
	 *	is appended.
	 *	\code
	 *	OByteArray dat;
	 *	/// Fill 'dat' with some data.
	 *	OByteList list = dat.chunkWithHeader(1000
	 *					[](OByteArray& array, int index, int size)->void{
	 *		//append a header to the chunks so when the data is sent over
	 *		//the network or serial port the receiving side knows how to 
	 *		//handle the packets
	 *		unsigned char packetType = 10;
	 *		unsigned short packetLength = size;
	 *		array<<packetLength <<packetType;
	 *	});
	 *	\endcode
	*/
	OList<OByteArray> chunkWithHeader(int size,
									  function<void (OByteArray&, int, int)> cbk);
	
	
	/**	This OByteArray is split into an array of pieces of
	 *	a specified size. A functor may be passes as the
	 *	second parameter. This functor must take 3 argurments.
	 *	The first arguement is the OByteArray for the chunk 
	 *	we are about to create. The second arguement is the
	 *	current index of the OByteArray for the chunk we are
	 *	about to create. The third arguement is the size of 
	 *	OByteArray for the chunk we are about to create.
	 *	@param fn The name of the file to load and split.
	 *	@param size The size of the pieces that we are making.
	 *	@param cbk The functor to be called so data may be packed
	 *	to to the beginning of the OByteArray before the chunk
	 *	is appended.
	 *	\code
	 *	OByteList list = OByteArray::chunkFileWithHeader("test.dat", 1000
	 *					[](OByteArray& array, int index, int size)->void{
	 *		//append a header to the chunks so when the data is sent over
	 *		//the network or serial port the receiving side knows how to 
	 *		//handle the packets
	 *		unsigned char packetType = 10;
	 *		unsigned short packetLength = size;
	 *		array<<packetLength <<packetType;
	 *	});
	 *	\endcode
	*/
	static OList<OByteArray> chunkFileWithHeader(OString fn, 
										  int size, 
										  function<void (OByteArray&, int, int)> cbk);
	
	
	/**	Read an arbitrary amount of data out of the OByteArray.
	 *	@param ptr The data to be read will be copied to this pointer.
	 *	@param len The length of the data to be read. If this value
	 *		is negative all the remaining data will be read.
	 *	@return The amount of data actually read.
	*/
	int read(char* ptr, int len);
	
	/**	Sum all the bytes of the data. This is useful if this data
	  *	is to be send over a serial port.
	 */
	int checksum() const;
	
	/**	Get the position of the stream pointer. Depending on the 
	 *	size of the data being read each call to 
	 *	OByteArray::operator>> will advance the stream pointer.
	 *	@return The position of the stream pointer in the
	 *	OByteArray.
	*/
	int tell() const;
	
	/**	Set the current position of the stream pointer.
	 *	For basic packing and unpacking of data into a 
	 *	OByteArray this function will almost never be used.
	 *	@param pos Set the stream pointer to this spot in the
	 *	OByteArray.
	*/
	void seek(int pos, OO::IOBase base = OO::beg);
	
	/**	Advance the stream pointer.
	 *	@param addition The amount by which to increment the
	 *	stream pointer.
	*/
	void advance(int addition);
	
	/**	Advance the size of the data in the array.
	 *	@param The amount by which to increment the recorded
	 *	amount of data in the binary array.
	*/
	void advanceSize(int addition);
	
	/**	Get a pointer to the data at the current position of the
	 *	stream pointer.
	*/
	char* tellData();
	
	/**	Get the amount of data after the current position of the
	 *	stream pointer.
	*/
	int dataLeft() const;
	
	/**	The amount of data being stored in this OByteArray.
	*/
	int size() const;
	
	/**	Increase the storage size of this OByteArray by len.
	 */
	void resize(int len);
	
	/**	Get a pointer to the beginning of the data.
	*/
	char* data();
	
	/**	Get a const pointer to the beginning of the data.
	*/
	const char* constData() const;
	
	/**	Clear the contents of this OByteArray.
	 */
	void clear();
	
protected:
	struct OByteArrayMem {
		OByteArrayMem(int len) {
			sizeofdata = 0;
			sizeofarray = len;
			bytearray.reset(new char[len]);
			streamptr = 0;
		}
		
		int sizeofdata;		//the amount of data being stored
		int sizeofarray;	//the size of the memory being pointed to
		unique_ptr<char> bytearray;	//a pointer to the data
		
		int streamptr;
		
		OO::ArrayBase dir;
	};
	
	shared_ptr<OByteArrayMem> mem;
	
	void makeOwner();
	
	void checkResize(int addition);
	
};

typedef OList<OByteArray> OByteList;

#endif // OByteArray_H

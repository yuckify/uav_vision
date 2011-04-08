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
#include<vector>

#include<boost/crc.hpp>
#include<boost/scoped_array.hpp>

#include<OString.hpp>

#ifdef OO_BOTAN
#include<botan/botan.h>
#include<botan/rsa.h>
#endif


#ifndef __windows__
	#include<tr1/functional>
	using namespace std::placeholders;
#endif

using namespace std;

//swap byte order short
#define sbos(n) ((((int16_t)(n) & 0xff)<<8)					| \
				 (((int16_t)(n) & 0xff00)>>8))

//swap byte order unsigned short
#define sbous(n) ((((uint16_t)(n) & 0xff)<<8)				| \
				  (((uint16_t)(n) & 0xff00)>>8))

//swap byte order int
#define sboi(n) ((((int32_t)(n) & 0x000000ff)<<24)			| \
				 (((int32_t)(n) & 0x0000ff00)<<8)			| \
				 (((int32_t)(n) & 0x00ff0000)>>8)			| \
				 (((int32_t)(n) & 0xff000000)>>24))

//swap byte order unsigned int
#define sboui(n) ((((uint32_t)(n) & 0x000000ff)<<24)		| \
				  (((uint32_t)(n) & 0x0000ff00)<<8)			| \
				  (((uint32_t)(n) & 0x00ff0000)>>8)			| \
				  (((uint32_t)(n) & 0xff000000)>>24))

//swap byte order long int
#define sboli(n) ((((int64_t)(n) & 0x00000000000000ff)<<56) | \
				  (((int64_t)(n) & 0x000000000000ff00)<<40) | \
				  (((int64_t)(n) & 0x0000000000ff0000)<<24) | \
				  (((int64_t)(n) & 0x00000000ff000000)<<8)	| \
				  (((int64_t)(n) & 0x000000ff00000000)>>8)	| \
				  (((int64_t)(n) & 0x0000ff0000000000)>>24) | \
				  (((int64_t)(n) & 0x00ff000000000000)>>40) | \
				  (((int64_t)(n) & 0xff00000000000000)>>56))

//swap byte order unsigned long int
#define sbouli(n) ((((uint64_t)(n) & 0x00000000000000ff)<<56) | \
				   (((uint64_t)(n) & 0x000000000000ff00)<<40) | \
				   (((uint64_t)(n) & 0x0000000000ff0000)<<24) | \
				   (((uint64_t)(n) & 0x00000000ff000000)<<8)  | \
				   (((uint64_t)(n) & 0x000000ff00000000)>>8)  | \
				   (((uint64_t)(n) & 0x0000ff0000000000)>>24) | \
				   (((uint64_t)(n) & 0x00ff000000000000)>>40) | \
				   (((uint64_t)(n) & 0xff00000000000000)>>56))

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
	friend class OTcpSocket;
	friend class OPipe;
	friend class OUdpSocket;
	friend class OSerial;
public:
	/**	Create a OByteArray with a default length of 10 or
	 *	n bytes;
	 *	@param n The length of the array to start with.
	 *	@param end The endianness of the data stored in this byte array.
	 *	@param p The amount of free data preceding the beinning of the 
	 *	data container. This is allocated so a couple small pieces of data
	 *	may be appended to the beginning of the array with no memory 
	 *	reallocation penalty. If large amounts of data are prepended
	 *	the binary data container will expand to accommodate.
	*/
	explicit OByteArray(OO::Endian end = OO::LittleEndian);
	
	/**	The copy constructor.
	 *	@param old The OByteArray obect being copied.
	*/
	OByteArray(const OByteArray& old);
	
	/**	Initialize a OByteArray with an arbitrary block of memory.
	 *	@param data The block to data to initialize with.
	 *	@param len The amount of data to be used in bytes.
	*/
	explicit OByteArray(const char* data, int len);
	
	/**	Initialize a OByteArray with a string.
	*/
	OByteArray(const char* str);
	
	/// This function is overloaded.
	OByteArray(const OString& str);
	
	/// This function is overloaded.
	OByteArray(OSerializable& ser, OO::Endian endian = OO::LittleEndian);
	
	~OByteArray();
	
	/**	To serialize/deserialize a class/struct it must inherit
	 *	OSerializable. The function serialize() must be defined.
	 *	This function ( operator&() ) is primarily intended to be used
	 *	inside the serialize() function inside a class. This 
	 *	interface allows the developer to write a function to easily 
	 *	recursively serialize/deserialize a class without being 
	 *	required to know which operation is performed.
	*/
	template <class T> OByteArray& operator&(T& item) {
		OByteArray& arr = *this;
		if(mem->dir == OO::Input) {
			arr<<item;
		} else {
			arr>>item;
		}
		return *this;
	}
	
	/**	This function writes a piece of data at the current position
	 *	of the stream pointer.
	*/
	void write(OByteArray& ba);
	
	/// This function is overloaded.
	void write(OByteArray&& ba);
	
	/// This function is overloaded.
	void write(const OString& str);
	
	/// This function is overloaded.
	void write(const OString&& str);
	
	/// This function is overloaded.
	void write(const string str);
	
	/// This function is overloaded.
	void write(const char* str);
	
	/// This function is overloaded.
	void write(const void* str, int len);
	
	/// This function is overloaded.
	void write(OSerializable& obj);
	
	/// This function is overloaded.
	void write(bool i);
	
	/// This function is overloaded.
	void write(int8_t i);
	
	/// This function is overloaded.
	void write(int16_t i);
	
	/// This function is overloaded.
	void write(int32_t i);
	
	/// This function is overloaded.
	void write(int64_t i);
	
	/// This function is overloaded.
	void write(uint8_t i);
	
	/// This function is overloaded.
	void write(uint16_t i);
	
	/// This function is overloaded.
	void write(uint32_t i);
	
	/// This function is overloaded.
	void write(uint64_t i);
	
	/// This function is overloaded.
	template <class T> void write(const vector<T>& vec) {
		OByteArray& arr = *this;
		uint32_t length = vec.size();
		arr<<length;
		for(unsigned int i=0; i<length; i++)
			arr<<(T&)(vec[i]);
	}
	
	/// This function is overloaded.
	template <class T, class U> void write(const map<T, U>& ma) {
		OByteArray& arr = *this;
		uint32_t length = ma.size();
		arr<<length;
		
		for_each(ma.begin(), ma.end(), [&arr] (const std::pair<T, U>& pa) {
			arr<<(T&)pa.first <<(U&)pa.second;
		});
	}
	
	/**	This function writes a piece of data at the current position
	 *	of the stream pointer. Calling this function is provided
	 *	for convenience to stream in a large number of objects. This
	 *	function is otherwise the same as calling write().
	 *	@see OByteArray::write()
	*/
	OByteArray& operator<<(OByteArray& ba);
	OByteArray& operator<<(OByteArray&& ba);
	OByteArray& operator<<(const OString& str);
	OByteArray& operator<<(const OString&& str);
	OByteArray& operator<<(string str);
	OByteArray& operator<<(const char* str);
	OByteArray& operator<<(OSerializable& obj);
	OByteArray& operator<<(bool i);
	OByteArray& operator<<(int8_t i);
	OByteArray& operator<<(int16_t i);
	OByteArray& operator<<(int32_t i);
	OByteArray& operator<<(int64_t i);
	OByteArray& operator<<(uint8_t i);
	OByteArray& operator<<(uint16_t i);
	OByteArray& operator<<(uint32_t i);
	OByteArray& operator<<(uint64_t i);
	
	/// This function is overloaded.
	template <class T> OByteArray& operator<<(const vector<T>& vec) {
		this->write(vec);
		return *this;
	}
	
	/// This function is overloaded.
	template <class T, class U> OByteArray& operator<<(const map<T, U>& ma) {
		this->write(ma);
		return *this;
	}
	
	/**	Append a piece of data to the very end of a binary array.
	 *	
	*/
	void append(OByteArray& ba);
	
	/// This function is overloaded.
	void append(OByteArray&& ba);
	
	/// This function is overloaded.
	void append(const OString& str);
	
	/// This function is overloaded.
	void append(const OString&& str);
	
	/// This function is overloaded.
	void append(const string str);
	
	/// This function is overloaded.
	void append(const char* str);
	
	/// This function is overloaded.
	void append(const char* str, int len);
	
	/// This function is overloaded.
	void append(OSerializable& obj);
	
	/// This function is overloaded.
	void append(bool i);
	
	/// This function is overloaded.
	void append(int8_t i);
	
	/// This function is overloaded.
	void append(int16_t i);
	
	/// This function is overloaded.
	void append(int32_t i);
	
	/// This function is overloaded.
	void append(int64_t i);
	
	/// This function is overloaded.
	void append(uint8_t i);
	
	/// This function is overloaded.
	void append(uint16_t i);
	
	/// This function is overloaded.
	void append(uint32_t i);
	
	/// This function is overloaded.
	void append(uint64_t i);
	
	/**	This function writes a piece of data at the end of the binary
	 *	array. Calling this function is provided for convenience to 
	 *	stream in a large number of objects. This function is otherwise 
	 *	the same as calling append().
	 *	@see OByteArray::append()
	*/
	template<class T> OByteArray& operator|(T& item) {
		append(item);
		return *this;
	}
	
	/**	Read a piece of data at the position of the stream pointer.
	*/
	void read(bool& i);
	
	/// This function is overloaded.
	void read(char& i);
	
	/// This function is overloaded.
	void read(int8_t& i);
	
	/// This function is overloaded.
	void read(int16_t& i);
	
	/// This function is overloaded.
	void read(int32_t& i);
	
	/// This function is overloaded.
	void read(int64_t& i);
	
	/// This function is overloaded.
	void read(uint8_t& i);
	
	/// This function is overloaded.
	void read(uint16_t& i);
	
	/// This function is overloaded.
	void read(uint32_t& i);
	
	/// This function is overloaded.
	void read(uint64_t& i);
	
	/// This function is overloaded.
	void read(OSerializable& obj);
	
	/// This function is overloaded.
	void read(OString& str);
	
	/// This function is overloaded.
	template <class T> void read(vector<T>& vec) {
		vec.clear();
		
		uint32_t len = 0;
		OByteArray& arr = *this;
		arr>>len;
		
		for(uint32_t i=0; i<len; i++) {
			T xfer;
			arr>>xfer;
			vec.push_back(xfer);
		}
	}
	
	/// This function is overloaded.
	template <class T, class U> void read(map<T, U>& ma) {
		ma.clear();
		
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
	
	/**	This function reads a piece of data at the position of the 
	 *	stream pointer. Calling this function is provided for convenience to 
	 *	stream out a large number of objects. This function is otherwise 
	 *	the same as calling read().
	 *	@see OByteArray::read()
	*/
	template<class T> OByteArray& operator>>(T& item) {
		read(item);
		return *this;
	}
	
#ifdef OO_BOTAN
	OByteArray encrypt(Botan::Public_Key* key);
	
	OByteArray decrypt(Botan::Public_Key* key);
#endif
	
	/**	Load the binary data into this data container.
	 *	@param fn The name of the file to load.
	*/
	bool loadFile(OString fn);
	
	/**	Save the binary data in this data container to a file.
	 *	@param fn The name of the file to save the data to.
	*/
	bool saveFile(OString fn);
	
	static OList<OByteArray> chunkData(const void* data, int dsize, int size);
	static OList<OByteArray> chunkDataWithHeader(const void* data, int dsize, int size,
												 function<void (OByteArray&, int, int)> cbk);
	
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
	 *	is writeed.
	 *	\code
	 *	OByteArray dat;
	 *	/// Fill 'dat' with some data.
	 *	OByteList list = dat.chunkWithHeader(1000
	 *					[](OByteArray& array, int index, int size)->void{
	 *		//write a header to the chunks so when the data is sent over
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
	 *	is writeed.
	 *	\code
	 *	OByteList list = OByteArray::chunkFileWithHeader("test.dat", 1000
	 *					[](OByteArray& array, int index, int size)->void{
	 *		//write a header to the chunks so when the data is sent over
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
	
	
	int find(const OByteArray& data, int start = 0);
	
	int find(const void* data, int length, int start = 0);
	
	/**	Read an arbitrary amount of data out of the OByteArray.
	 *	@param ptr The data to be read will be copied to this pointer.
	 *	@param len The length of the data to be read. If this value
	 *		is negative all the remaining data will be read.
	 *	@return The amount of data actually read.
	*/
	int read(void* ptr, int len);
	
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
	
	/**	Get the amount of data after the current position of the
	 *	stream pointer.
	*/
	unsigned dataLeft() const;
	
	/**	The amount of data being stored in this OByteArray.
	*/
	unsigned size() const;
	
	/**	Set the storage size of this OByteArray by len.
	 */
	void resize(int len);
	
	/**	Increase the storage size of this OByteArray by len.
	 */
	void enlarge(int len);
	
	/**	Get a pointer to the beginning of the data.
	*/
	unsigned char* data();
	
	/**	Get a const pointer to the beginning of the data.
	*/
	const unsigned char* constData() const;
	
	/**	Clear the contents of this OByteArray.
	 */
	void clear();
	
	/**	Get the endianness of the data stored in the array.
	*/
	OO::Endian endian() const;
	
	/**	Set the endianness of the data stored in the array.
	 *	Setting the endianness effects the next read and
	 *	write operations.
	*/
	void setEndian(OO::Endian end);
	
	typedef unsigned char* Iterator;
	typedef const unsigned char* ConstIterator;
	Iterator end();
	Iterator begin();
	ConstIterator end() const;
	ConstIterator begin() const;
	
	/**	Get a pointer to the data at the current position of the
	 *	stream pointer.
	*/
	Iterator tellData();
	
	ConstIterator tellData() const;
	
	ConstIterator constTellData() const;
	
	
protected:
	
	void seekCurrent(int len);
	void seekBegin(int len);
	void seekEnd(int len);
	
	/// Seek function switch.
	static void (OByteArray::*seekSwitch[])(int);
	
	/**	Advance the size of the data in the array.
	 *	@param addition If the data being written exceeds the
	 *	current amount of data in the array, the amount of 
	 *	recorded data will be increased to reflect the write.
	*/
//	void advanceSize(int addition);
	
#if defined(__LITTLE_ENDIAN__)	|| \
	defined(i686)				|| \
	defined(__i686)				|| \
	defined(__i686__)			|| \
	defined(i386)				|| \
	defined(__i386)				|| \
	defined(__i386__)
	
	//host byte ordering to little endian
	int16_t htole(int16_t i) { return i; }
	uint16_t htole(uint16_t i) { return i; }
	
	int32_t htole(int32_t i) { return i; }
	uint32_t htole(uint32_t i) { return i; }
	
	int64_t htole(int64_t i) { return i; }
	uint64_t htole(uint64_t i) { return i; }
	
	//host byte ordering to big endian
	int16_t htobe(int16_t i) { return sbos(i); }
	uint16_t htobe(uint16_t i) { return sbous(i); }
	
	int32_t htobe(int32_t i) { return sboi(i); }
	uint32_t htobe(uint32_t i) { return sboui(i); }
	
	int64_t htobe(int64_t i) { return sboli(i); }
	uint64_t htobe(uint64_t i) { return sbouli(i); }
	
	//little endian to host byte ordering
	int16_t letoh(int16_t i) { return i; }
	uint16_t letoh(uint16_t i) { return i; }
	
	int32_t letoh(int32_t i) { return i; }
	uint32_t letoh(uint32_t i) { return i; }
	
	int64_t letoh(int64_t i) { return i; }
	uint64_t letoh(uint64_t i) { return i; }
	
	//big endian to host byte ordering
	int16_t betoh(int16_t i) { return sbos(i); }
	uint16_t betoh(uint16_t i) { return sbous(i); }
	
	int32_t betoh(int32_t i) { return sboi(i); }
	uint32_t betoh(uint32_t i) { return sboui(i); }
	
	int64_t betoh(int64_t i) { return sboli(i); }
	uint64_t betoh(uint64_t i) { return sbouli(i); }
	
#else
	
	//host byte ordering to little endian
	int16_t htole(int16_t i) { return sbos(i); }
	uint16_t htole(uint16_t i) { return sbous(i); }
	
	int32_t htole(int32_t i) { return sboi(i); }
	uint32_t htole(uint32_t i) { return sboui(i); }
	
	int64_t htole(int64_t i) { return sboli(i); }
	uint64_t htole(uint64_t i) { return sbouli(i); }
	
	//host byte ordering to big endian
	int16_t htobe(int16_t i) { return i; }
	uint16_t htobe(uint16_t i) { return i; }
	
	int32_t htobe(int32_t i) { return i; }
	uint32_t htobe(uint32_t i) { return i; }
	
	int64_t htobe(int64_t i) { return i; }
	uint64_t htobe(uint64_t i) { return i; }
	
	//little endian to host byte ordering
	int16_t letoh(int16_t i) { return sbos(i); }
	uint16_t letoh(uint16_t i) { return sbous(i); }
	
	int32_t letoh(int32_t i) { return sboi(i); }
	uint32_t letoh(uint32_t i) { return sboui(i); }
	
	int64_t letoh(int64_t i) { return sboli(i); }
	uint64_t letoh(uint64_t i) { return sbouli(i); }
	
	//big endian to host byte ordering
	int16_t betoh(int16_t i) { return i; }
	uint16_t betoh(uint16_t i) { return i; }
	
	int32_t betoh(int32_t i) { return i; }
	uint32_t betoh(uint32_t i) { return i; }
	
	int64_t v(int64_t i) { return i; }
	uint64_t betoh(uint64_t i) { return i; }
	
#endif
	
	
	struct OByteArrayMem {
		OByteArrayMem() {
			end = OO::LittleEndian;
		}
		
		vector<unsigned char> bytes;
		
		OO::Endian end;
		OO::ArrayBase dir;
	};
	
	int streamptr;
	
	shared_ptr<OByteArrayMem> mem;
	
	void makeOwner();
};

typedef OList<OByteArray> OByteList;

#endif // OByteArray_H

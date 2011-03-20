#include"OByteArray.hpp"

OByteArray::OByteArray(int n) : 
		mem(new OByteArrayMem(n)) {
	streamptr = 0;
}

OByteArray::OByteArray(const OByteArray& old) {
	mem = old.mem;
	streamptr = old.streamptr;
}

OByteArray::OByteArray(const char* data, int len) : 
		mem(new OByteArrayMem(len)) {
	streamptr = 0;
	
	::memcpy(mem->bytearray.get(), data, len);
	advanceSize(len);
}

OByteArray::OByteArray(const char *str) {
	streamptr = 0;
	int strlen = 0;
	const char* strptr = str;
	while(*strptr++)
		strlen++;
	
	mem.reset(new OByteArrayMem(strlen+1));
	::memcpy(mem->bytearray.get(), str, strlen+1);
	
	advanceSize(strlen+1);
}

OByteArray::OByteArray(const OString &str) {
	streamptr = 0;
	
	mem.reset(new OByteArrayMem(str.length()+1));
	::memcpy(mem->bytearray.get(), str.toCString(), str.length()+1);
	
	advanceSize(str.length()+1);
}

OByteArray::~OByteArray() {
	mem.reset();
}

void OByteArray::append(OByteArray& ba) {
	append(ba.data(), ba.size());
}

void OByteArray::append(OByteArray&& ba) {
	append(ba.data(), ba.size());
}

void OByteArray::append(const string str) {
	append(str.c_str(), str.length()+1);
}

void OByteArray::append(const char* str) {
	int strlen = 0;
	const char* strptr = str;
	while(*strptr++)
		strlen++;
	
	append(str, strlen+1);
}

void OByteArray::append(const OString& str) {
	append(str.c_str(), str.length()+1);
}

void OByteArray::append(const OString&& str) {
	append(str.c_str(), str.length()+1);
}

void OByteArray::append(const char* str, int len) {
	if(!len) return;
	checkResize(len);
	::memcpy(this->tellData(), str, len);
	if(streamptr == mem->sizeofdata)
		mem->sizeofdata += len;
	streamptr += len;
}

void OByteArray::append(OSerializable &obj) {
	makeOwner();
	
	mem->dir = OO::Input;
	obj.serialize(*this, mem->dir);
}

void OByteArray::append(bool i) {
	register int len = sizeof(i);
	checkResize(len);
	bool* tmpptr = (bool*)this->tellData();
	*tmpptr = i;
	if(streamptr == mem->sizeofdata)
		mem->sizeofdata += len;
	streamptr += len;
}

void OByteArray::append(int8_t i) {
	register int len = sizeof(i);
	checkResize(len);
	int8_t* tmpptr = (int8_t*)this->tellData();
	*tmpptr = i;
	if(streamptr == mem->sizeofdata)
		mem->sizeofdata += len;
	streamptr += len;
}

void OByteArray::append(int16_t i) {
	register int len = sizeof(i);
	checkResize(len);
	int16_t* tmpptr = (int16_t*)this->tellData();
	*tmpptr = i;
	if(streamptr == mem->sizeofdata)
		mem->sizeofdata += len;
	streamptr += len;
}

void OByteArray::append(int32_t i) {
	register int len = sizeof(i);
	checkResize(len);
	int32_t* tmpptr = (int32_t*)this->tellData();
	*tmpptr = i;
	if(streamptr == mem->sizeofdata)
		mem->sizeofdata += len;
	streamptr += len;
}

void OByteArray::append(int64_t i) {
	register int len = sizeof(i);
	checkResize(len);
	int64_t* tmpptr = (int64_t*)this->tellData();
	*tmpptr = i;
	if(streamptr == mem->sizeofdata)
		mem->sizeofdata += len;
	streamptr += len;
}

void OByteArray::append(uint8_t i) {
	register int len = sizeof(i);
	checkResize(len);
	uint8_t* tmpptr = (uint8_t*)this->tellData();
	*tmpptr = i;
	if(streamptr == mem->sizeofdata)
		mem->sizeofdata += len;
	streamptr += len;
}

void OByteArray::append(uint16_t i) {
	register int len = sizeof(i);
	checkResize(len);
	uint16_t* tmpptr = (uint16_t*)this->tellData();
	*tmpptr = i;
	if(streamptr == mem->sizeofdata)
		mem->sizeofdata += len;
	streamptr += len;
}

void OByteArray::append(uint32_t i) {
	register int len = sizeof(i);
	checkResize(len);
	uint32_t* tmpptr = (uint32_t*)this->tellData();
	*tmpptr = i;
	if(streamptr == mem->sizeofdata)
		mem->sizeofdata += len;
	streamptr += len;
}

void OByteArray::append(uint64_t i) {
	register int len = sizeof(i);
	checkResize(len);
	uint64_t* tmpptr = (uint64_t*)this->tellData();
	*tmpptr = i;
	if(streamptr == mem->sizeofdata)
		mem->sizeofdata += len;
	streamptr += len;
}

OByteArray& OByteArray::operator<<(bool i) {
	append(i);
	return *this;
}

OByteArray& OByteArray::operator<<(int8_t i) {
	append(i);
	return *this;
}

OByteArray& OByteArray::operator<<(int16_t i) {
	append(i);
	return *this;
}

OByteArray& OByteArray::operator<<(int32_t i) {
	append(i);
	return *this;
}

OByteArray& OByteArray::operator<<(int64_t i) {
	append(i);
	return *this;
}

OByteArray& OByteArray::operator<<(uint8_t i) {
	append(i);
	return *this;
}

OByteArray& OByteArray::operator<<(uint16_t i) {
	append(i);
	return *this;
}

OByteArray& OByteArray::operator<<(uint32_t i) {
	append(i);
	return *this;
}

OByteArray& OByteArray::operator<<(uint64_t i) {
	append(i);
	return *this;
}

OByteArray& OByteArray::operator<<(OSerializable &obj) {
	append(obj);
	return *this;
}

OByteArray& OByteArray::operator<<(const char* str) {
	append(str);
	return *this;
}

OByteArray& OByteArray::operator<<(string& str) {
	append(str);
	return *this;
}

OByteArray& OByteArray::operator<<(OString& str) {
	append(str);
	return *this;
}

OByteArray& OByteArray::operator<<(OString&& str) {
	append(str);
	return *this;
}

OByteArray& OByteArray::operator<<(OByteArray& data) {
	append(data);
	return *this;
}

OByteArray& OByteArray::operator >>(char& i) {
	char* tmpptr = (char*)(mem->bytearray.get() + streamptr);
	streamptr += sizeof(i);
	i = *tmpptr;
	return *this;
}

OByteArray& OByteArray::operator >>(bool& i) {
	bool* tmpptr = (bool*)(mem->bytearray.get() + streamptr);
	streamptr += sizeof(i);
	i = *tmpptr;
	return *this;
}

OByteArray& OByteArray::operator>>(uint8_t& i) {
	uint8_t* tmpptr = (uint8_t*)(mem->bytearray.get() + streamptr);
	streamptr += sizeof(i);
	i = *tmpptr;
	return *this;
}

OByteArray& OByteArray::operator>>(uint16_t& i) {
	uint16_t* tmpptr = (uint16_t*)(mem->bytearray.get() + streamptr);
	streamptr += sizeof(i);
	i = *tmpptr;
	return *this;
}

OByteArray& OByteArray::operator>>(uint32_t& i) {
	uint32_t* tmpptr = (uint32_t*)(mem->bytearray.get() + streamptr);
	streamptr += sizeof(i);
	i = *tmpptr;
	return *this;
}

OByteArray& OByteArray::operator>>(uint64_t& i) {
	uint64_t* tmpptr = (uint64_t*)(mem->bytearray.get() + streamptr);
	streamptr += sizeof(i);
	i = *tmpptr;
	return *this;
}

OByteArray& OByteArray::operator>>(int8_t& i) {
	int8_t* tmpptr = (int8_t*)(mem->bytearray.get() + streamptr);
	streamptr += sizeof(i);
	i = *tmpptr;
	return *this;
}

OByteArray& OByteArray::operator>>(int16_t& i) {
	int16_t* tmpptr = (int16_t*)(mem->bytearray.get() + streamptr);
	streamptr += sizeof(i);
	i = *tmpptr;
	return *this;
}

OByteArray& OByteArray::operator>>(int32_t& i) {
	int32_t* tmpptr = (int32_t*)(mem->bytearray.get() + streamptr);
	streamptr += sizeof(i);
	i = *tmpptr;
	return *this;
}

OByteArray& OByteArray::operator>>(int64_t& i) {
	int64_t* tmpptr = (int64_t*)(mem->bytearray.get() + streamptr);
	streamptr += sizeof(i);
	i = *tmpptr;
	return *this;
}

OByteArray& OByteArray::operator>>(OSerializable& obj) {
	mem->dir = OO::Output;
	
	obj.serialize(*this, mem->dir);
	return *this;
}

OByteArray& OByteArray::operator>>(OString& str) {
	OString tmpstr(mem->bytearray.get() + streamptr);
	
	while(mem->bytearray.get()[streamptr++]) {}
	str = tmpstr;
	return *this;
}

bool OByteArray::loadFile(OString fn) {
	makeOwner();
	
	//open the file
	fstream file;
	file.open(fn.toCString(), ios_base::in);
	
	//check to see if the file exists, else return error
	if(!file.is_open())
		return false;
	
	//get the size of the file
	file.seekg(0, ios_base::end);
	int length = file.tellg();
	file.seekg(0, ios_base::beg);
	
	//make sure this container can store the entire file
	resize(length);
	
	//read in the data
	file.read(data(), length);
	
	//close the file since we are done with it
	file.close();
	
	return true;
}

bool OByteArray::saveFile(OString fn) {
	//open the file
	fstream file;
	file.open(fn.toCString(), ios_base::out);
	
	//check to see if the file has been opened, else return error
	if(!file.is_open()) {
		return false;
	}
	
	//write the data
	file.write(this->data(), this->size());
	
	//we are done here so close the file
	file.close();
	
	return true;
}

OList<OByteArray> OByteArray::chunkData(const char *data, int dsize, int size) {
	return chunkDataWithHeader(data, dsize, size, NULL);
}

OList<OByteArray> OByteArray::chunkDataWithHeader(const char *data, int dsize, int size, 
												  function<void (OByteArray &, int, int)> cbk) {
	//calculate the number of whole chunks, don't forget the remainder
	int chunks = dsize / size;
	int remainder = dsize % size;
	
	//the data we will be returning
	OList<OByteArray> bytes;
	
	//copy over the whole chunks
	for(int i=0; i<chunks; i++) {
		OByteArray array;
		if(cbk) cbk(array, i, size);
		array.append(data+i*size, size);
		bytes.push_back(array);
	}
	
	//append on the remainder
	if(remainder) {
		OByteArray array;
		if(cbk) cbk(array, chunks, remainder);
		array.append(data+chunks*size, remainder);
		bytes.push_back(array);
	}
	
	return bytes;
}

OList<OByteArray> OByteArray::chunk(int size) {
	return chunkWithHeader(size, NULL);
}

OList<OByteArray> OByteArray::chunkWithHeader(int size, 
        function<void (OByteArray &, int, int)> cbk) {
	return chunkDataWithHeader(this->data(), this->size(), size, cbk);
}

OList<OByteArray> OByteArray::chunkFileWithHeader(OString fn, 
        int size,
        function<void (OByteArray &, int, int)> cbk) {
	OList<OByteArray> bytes;
	
	//open the file
	fstream file;
	file.open(fn.toCString(), ios_base::in);
	
	//check to see if the file has been opened, else return empty data
	if(!file.is_open()) {
		return bytes;
	}
	
	//get the length of data we will be reading in from the file
	file.seekg(0, ios_base::end);
	int length = file.tellg();
	file.seekg(0, ios_base::beg);
	
	//calculate the number of whole chunks, don't forget the remainder
	int chunks = length / size;
	int remainder = length % size;
	
	//copy over the whole chunks
	for(int i=0; i<chunks; i++) {
		OByteArray array;
		if(cbk) cbk(array, i, size);
		array.resize(length);
		file.read(array.tellData(), size);
		array.advanceSize(size);
		bytes.push_back(array);
	}
	
	//append the remainder
	if(remainder) {
		OByteArray array;
		if(cbk) cbk(array, chunks, remainder);
		array.resize(length);
		file.read(array.tellData(), remainder);
		array.advanceSize(remainder);
		bytes.push_back(array);
	}
	
	return bytes;
}

int OByteArray::find(const OByteArray &data, int start) {
	return find(data.constData(), data.size(), start);
}

int OByteArray::find(const char *data, int length, int start) {
	if(start + length > this->size()) return -1;
	
	bool equal = false;
	
	//do the search for the data
	int ending = this->size() - (start + length) + 1;
	for(int i=start; i<ending; i++) {
		if(!::memcmp(data, this->constData() + i, length)) {
			return i;
		}
	}
	
	return -1;
}

int OByteArray::read(char* ptr, int len) {
	int readlen = 0;
	if(len > dataLeft()) readlen = dataLeft();
	else readlen = len;
	::memcpy(ptr, tellData(), readlen);
	advance(readlen);
	return readlen;
}

int OByteArray::checksum() const {
	int length = size();
	register int sum = 0;
	register unsigned char* ptr = (unsigned char*)mem->bytearray.get();
	for(register int i=0; i<length; i++) sum += ptr[i];
	return sum;
}

int OByteArray::tell() const {
	return streamptr;
}

void OByteArray::seek(int pos, OO::IOBase base) {
	switch(base) {
		case OO::beg: {
			if(pos < 0)		streamptr = 0;
			else			streamptr = pos;
			break;
		}
		case OO::end: {
			int newpos = pos + mem->sizeofdata + streamptr;
			if(newpos < 0)	streamptr = 0;
			else			streamptr = newpos;
			break;
		}
		case OO::cur: {
			int newpos = pos + streamptr;
			if(newpos < 0)	streamptr = 0;
			else			streamptr = newpos;
			break;
		}
	}
	
	if(streamptr > mem->sizeofdata) streamptr = mem->sizeofdata;
	else if(streamptr < 0) streamptr = 0;
}

unsigned OByteArray::size() const {
	return mem->sizeofdata;
}

void OByteArray::advance(int addition) {
	streamptr += addition;
}

void OByteArray::advanceSize(int addition) {
	makeOwner();
	mem->sizeofdata += addition;
}

char* OByteArray::tellData() {
	return mem->bytearray.get() + streamptr;
}

int OByteArray::dataLeft() const {
	return mem->sizeofdata - streamptr;
}

void OByteArray::resize(int len) {
	checkResize(len);
}

char* OByteArray::data() {
	makeOwner();
	return mem->bytearray.get();
}

const char* OByteArray::constData() const {
	return mem->bytearray.get();
}

void OByteArray::clear() {
	mem->sizeofdata = 0;
	streamptr = 0;
}

void OByteArray::makeOwner() {
	if(mem.use_count() > 1) {
		OByteArrayMem* newmem = new OByteArrayMem(mem->sizeofdata);
		::memcpy(newmem->bytearray.get(), mem->bytearray.get(), mem->sizeofdata);
		newmem->sizeofdata = mem->sizeofdata;
		streamptr = streamptr;
		mem.reset(newmem);
	}
}

void OByteArray::checkResize(int addition) {
	//if the stream pointer is greater than the sizeofdata variable
	//then adjust the addition variable to reflect this so we allocate
	//the corrent ammount of memory
	int check = streamptr + addition - mem->sizeofdata;
	if(check > 0) addition = check;
	
	if(mem.use_count() > 1) {
		OByteArrayMem* newmem = new OByteArrayMem(addition+mem->sizeofdata);
		::memcpy(newmem->bytearray.get(), mem->bytearray.get(), mem->sizeofdata);
		newmem->sizeofdata = mem->sizeofdata;
		streamptr = streamptr;
		mem.reset(newmem);
	} else if((addition + mem->sizeofdata) > mem->sizeofarray) {
		int mult = 2;
		while((addition + mem->sizeofdata) > (mem->sizeofarray * mult)) mult *= 2;
		mem->sizeofarray *= mult;
		char* tmparray = new char[mem->sizeofarray];
		::memcpy(tmparray, mem->bytearray.get(), mem->sizeofdata);
		mem->bytearray.reset(tmparray);
		tmparray = 0;
	}
}

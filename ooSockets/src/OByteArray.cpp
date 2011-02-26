#include"OByteArray.hpp"

OByteArray::OByteArray(int n) : 
		mem(new OByteArrayMem(n)) {
	
}

OByteArray::OByteArray(const OByteArray& old) {
	mem = old.mem;
}

OByteArray::OByteArray(const char* data, int len) : 
		mem(new OByteArrayMem(len)) {
	
	::memcpy(mem->bytearray.get(), data, len);
}

OByteArray::OByteArray(const char *str) {
	int strlen = 0;
	const char* strptr = str;
	while(*strptr++)
		strlen++;
	
	mem.reset(new OByteArrayMem(strlen+1));
	::memcpy(mem->bytearray.get(), str, strlen+1);
	
	advanceSize(strlen+1);
}

OByteArray::OByteArray(const OString &str) {
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
	checkResize(len);
	::memcpy(mem->bytearray.get() + mem->streamptr, str, len);
	mem->sizeofdata += len;
	mem->streamptr += len;
}

void OByteArray::append(OSerializable &obj) {
	makeOwner();
	
	mem->dir = OO::Input;
	obj.serialize(*this, mem->dir);
}

void OByteArray::append(bool i) {
	register int len = sizeof(i);
	checkResize(len);
	bool* tmpptr = (bool*)(&mem->bytearray.get()[mem->streamptr]);
	*tmpptr = i;
	if(mem->streamptr == mem->sizeofdata)
		mem->sizeofdata += len;
	mem->streamptr += len;
}

void OByteArray::append(int8_t i) {
	register int len = sizeof(i);
	checkResize(len);
	int8_t* tmpptr = (int8_t*)(&mem->bytearray.get()[mem->streamptr]);
	*tmpptr = i;
	if(mem->streamptr == mem->sizeofdata)
		mem->sizeofdata += len;
	mem->streamptr += len;
}

void OByteArray::append(int16_t i) {
	register int len = sizeof(i);
	checkResize(len);
	int16_t* tmpptr = (int16_t*)(&mem->bytearray.get()[mem->streamptr]);
	*tmpptr = i;
	if(mem->streamptr == mem->sizeofdata)
		mem->sizeofdata += len;
	mem->streamptr += len;
}

void OByteArray::append(int32_t i) {
	register int len = sizeof(i);
	checkResize(len);
	int32_t* tmpptr = (int32_t*)(&mem->bytearray.get()[mem->streamptr]);
	*tmpptr = i;
	if(mem->streamptr == mem->sizeofdata)
		mem->sizeofdata += len;
	mem->streamptr += len;
}

void OByteArray::append(int64_t i) {
	register int len = sizeof(i);
	checkResize(len);
	int64_t* tmpptr = (int64_t*)(&mem->bytearray.get()[mem->streamptr]);
	*tmpptr = i;
	if(mem->streamptr == mem->sizeofdata)
		mem->sizeofdata += len;
	mem->streamptr += len;
}

void OByteArray::append(uint8_t i) {
	register int len = sizeof(i);
	checkResize(len);
	uint8_t* tmpptr = (uint8_t*)(&mem->bytearray.get()[mem->streamptr]);
	*tmpptr = i;
	if(mem->streamptr == mem->sizeofdata)
		mem->sizeofdata += len;
	mem->streamptr += len;
}

void OByteArray::append(uint16_t i) {
	register int len = sizeof(i);
	checkResize(len);
	uint16_t* tmpptr = (uint16_t*)(&mem->bytearray.get()[mem->streamptr]);
	*tmpptr = i;
	if(mem->streamptr == mem->sizeofdata)
		mem->sizeofdata += len;
	mem->streamptr += len;
}

void OByteArray::append(uint32_t i) {
	register int len = sizeof(i);
	checkResize(len);
	uint32_t* tmpptr = (uint32_t*)(&mem->bytearray.get()[mem->streamptr]);
	*tmpptr = i;
	if(mem->streamptr == mem->sizeofdata)
		mem->sizeofdata += len;
	mem->streamptr += len;
}

void OByteArray::append(uint64_t i) {
	register int len = sizeof(i);
	checkResize(len);
	uint64_t* tmpptr = (uint64_t*)(&mem->bytearray.get()[mem->streamptr]);
	*tmpptr = i;
	if(mem->streamptr == mem->sizeofdata)
		mem->sizeofdata += len;
	mem->streamptr += len;
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

OByteArray& OByteArray::operator>>(bool i) {
	makeOwner();
	bool* tmpptr = (bool*)(mem->bytearray.get() + mem->streamptr);
	mem->streamptr += sizeof(i);
	i = *tmpptr;
	return *this;
}

OByteArray& OByteArray::operator>>(uint8_t& i) {
	makeOwner();
	uint8_t* tmpptr = (uint8_t*)(mem->bytearray.get() + mem->streamptr);
	mem->streamptr += sizeof(i);
	i = *tmpptr;
	return *this;
}

OByteArray& OByteArray::operator>>(uint16_t& i) {
	makeOwner();
	uint16_t* tmpptr = (uint16_t*)(mem->bytearray.get() + mem->streamptr);
	mem->streamptr += sizeof(i);
	i = *tmpptr;
	return *this;
}

OByteArray& OByteArray::operator>>(uint32_t& i) {
	makeOwner();
	uint32_t* tmpptr = (uint32_t*)(mem->bytearray.get() + mem->streamptr);
	mem->streamptr += sizeof(i);
	i = *tmpptr;
	return *this;
}

OByteArray& OByteArray::operator>>(uint64_t& i) {
	makeOwner();
	uint64_t* tmpptr = (uint64_t*)(mem->bytearray.get() + mem->streamptr);
	mem->streamptr += sizeof(i);
	i = *tmpptr;
	return *this;
}

OByteArray& OByteArray::operator>>(int8_t& i) {
	makeOwner();
	int8_t* tmpptr = (int8_t*)(mem->bytearray.get() + mem->streamptr);
	mem->streamptr += sizeof(i);
	i = *tmpptr;
	return *this;
}

OByteArray& OByteArray::operator>>(int16_t& i) {
	makeOwner();
	int16_t* tmpptr = (int16_t*)(mem->bytearray.get() + mem->streamptr);
	mem->streamptr += sizeof(i);
	i = *tmpptr;
	return *this;
}

OByteArray& OByteArray::operator>>(int32_t& i) {
	makeOwner();
	int32_t* tmpptr = (int32_t*)(mem->bytearray.get() + mem->streamptr);
	mem->streamptr += sizeof(i);
	i = *tmpptr;
	return *this;
}

OByteArray& OByteArray::operator>>(int64_t& i) {
	makeOwner();
	int64_t* tmpptr = (int64_t*)(mem->bytearray.get() + mem->streamptr);
	mem->streamptr += sizeof(i);
	i = *tmpptr;
	return *this;
}

OByteArray& OByteArray::operator>>(OSerializable& obj) {
	makeOwner();
	
	mem->dir = OO::Output;
	
	obj.serialize(*this, mem->dir);
	return *this;
}

OByteArray& OByteArray::operator>>(OString& str) {
	makeOwner();
	OString tmpstr(mem->bytearray.get() + mem->streamptr);
	
	while(mem->bytearray.get()[mem->streamptr++]) {}
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
	
	/*
	//calculate the number of whole chunks, don't forget the remainder
	int chunks = this->size() / size;
	int remainder = this->size() % size;
	
	//the data we will be returning
	OList<OByteArray> bytes;
	
	//copy over the whole chunks
	for(int i=0; i<chunks; i++) {
		OByteArray array;
		if(cbk) cbk(array, i, size);
		array.append(this->data()+i*size, size);
		bytes.push_back(array);
	}
	
	//append on the remainder
	if(remainder) {
		OByteArray array;
		if(cbk) cbk(array, chunks, remainder);
		array.append(this->data()+chunks*size, remainder);
		bytes.push_back(array);
	}
	
	return bytes;
	*/
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

int OByteArray::read(char* ptr, int len) {
	makeOwner();
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
	return mem->streamptr;
}

void OByteArray::seek(int pos, OO::IOBase base) {
	makeOwner();
	
	switch(base) {
		case OO::beg: {
			if(pos < 0)		mem->streamptr = 0;
			else			mem->streamptr = pos;
			break;
		}
		case OO::end: {
			int newpos = pos + mem->sizeofdata + mem->streamptr;
			if(newpos < 0)	mem->streamptr = 0;
			else			mem->streamptr = newpos;
			break;
		}
		case OO::cur: {
			int newpos = pos + mem->streamptr;
			if(newpos < 0)	mem->streamptr = 0;
			else			mem->streamptr = newpos;
			break;
		}
	}
}

int OByteArray::size() const {
	return mem->sizeofdata;
}

void OByteArray::advance(int addition) {
	makeOwner();
	mem->streamptr += addition;
}

void OByteArray::advanceSize(int addition) {
	makeOwner();
	mem->sizeofdata += addition;
}

char* OByteArray::tellData() {
	return mem->bytearray.get() + mem->streamptr;
}

int OByteArray::dataLeft() const {
	return mem->sizeofdata - mem->streamptr;
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
	mem->streamptr = 0;
}

void OByteArray::makeOwner() {
	if(mem.use_count() > 1) {
		OByteArrayMem* newmem = new OByteArrayMem(mem->sizeofdata);
		::memcpy(newmem->bytearray.get(), mem->bytearray.get(), mem->sizeofdata);
		newmem->sizeofdata = mem->sizeofdata;
		newmem->streamptr = mem->streamptr;
		mem.reset(newmem);
	}
}

void OByteArray::checkResize(int addition) {
	//if the stream pointer is greater than the sizeofdata variable
	//then adjust the addition variable to reflect this so we allocate
	//the corrent ammount of memory
	int check = mem->streamptr + addition - mem->sizeofdata;
	if(check > 0) addition = check;
	
	if(mem.use_count() > 1) {
		OByteArrayMem* newmem = new OByteArrayMem(addition+mem->sizeofdata);
		::memcpy(newmem->bytearray.get(), mem->bytearray.get(), mem->sizeofdata);
		newmem->sizeofdata = mem->sizeofdata;
		newmem->streamptr = mem->streamptr;
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

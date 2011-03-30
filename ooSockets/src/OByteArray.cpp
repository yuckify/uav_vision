#include"OByteArray.hpp"

void (OByteArray::*OByteArray::seekSwitch[])(int) = {&OByteArray::seekEnd, 
													 &OByteArray::seekBegin, 
													 &OByteArray::seekCurrent};

OByteArray::OByteArray(int n, OO::Endian end, int p) : 
		mem(new OByteArrayMem(n, p)) {
	mem->end = end;
	streamptr = 0;
}

OByteArray::OByteArray(const OByteArray& old) {
	mem = old.mem;
	streamptr = old.streamptr;
}

OByteArray::OByteArray(const char* data, int len) : 
		mem(new OByteArrayMem(len)) {
	streamptr = 0;
	
	mem->bytes.insert(mem->bytes.end(), data, data+len);
}

OByteArray::OByteArray(const char *str) {
	streamptr = 0;
	int strlen = 0;
	const char* strptr = str;
	while(*strptr++)
		strlen++;
	
	mem.reset(new OByteArrayMem(strlen+1));
	write(str);
}

OByteArray::OByteArray(const OString &str) {
	streamptr = 0;
	
	mem.reset(new OByteArrayMem(str.length()+1));
	write(str);
}

OByteArray::OByteArray(OSerializable &ser, OO::Endian endian) {
	mem.reset(new OByteArrayMem(20));
	streamptr = 0;
	mem->end = endian;
	
	this->write(ser);
}

OByteArray::~OByteArray() {
	mem.reset();
}

void OByteArray::write(OByteArray& ba) {
	write(ba.data(), ba.size());
}

void OByteArray::write(OByteArray&& ba) {
	write(ba.data(), ba.size());
}

void OByteArray::write(const string str) {
	write(str.c_str(), str.length()+1);
}

void OByteArray::write(const char* str) {
	int strlen = 0;
	const char* strptr = str;
	while(*strptr++)
		strlen++;
	
	write(str, strlen+1);
}

void OByteArray::write(const OString& str) {
	write(str.c_str(), str.length()+1);
}

void OByteArray::write(const OString&& str) {
	write(str.c_str(), str.length()+1);
}

void OByteArray::write(const void* str, int len) {
	if(len <= 0) return;
	
	mem->bytes.insert((vector<unsigned char>::iterator)tellData(), 
					  (unsigned char*)str, (unsigned char*)str+len+1);
	
	this->seek(len, OO::cur);
}

void OByteArray::write(OSerializable &obj) {
	makeOwner();
	
	mem->dir = OO::Input;
	obj.serialize(*this, mem->dir);
}

void OByteArray::write(bool i) {
	mem->bytes.insert((vector<unsigned char>::iterator)tellData(), *((const unsigned char*)&i), 
					  *((const unsigned char*)&i+sizeof(i)));
	
	this->seek(sizeof(i), OO::cur);
}

void OByteArray::write(int8_t i) {
	mem->bytes.insert((vector<unsigned char>::iterator)tellData(), *((const unsigned char*)&i), 
					  *((const unsigned char*)&i+sizeof(i)));
	
	this->seek(sizeof(i), OO::cur);
}

void OByteArray::write(int16_t i) {
	//check if we need to swap the byte ordering
	if(mem->end == OO::LittleEndian)	i = htole(i);
	else								i = htobe(i);
	
	mem->bytes.insert((vector<unsigned char>::iterator)tellData(), *((const unsigned char*)&i), 
					  *((const unsigned char*)&i+sizeof(i)));
	
	
	this->seek(sizeof(i), OO::cur);
}

void OByteArray::write(int32_t i) {
	//check if we need to swap the byte ordering
	if(mem->end == OO::LittleEndian)	i = htole(i);
	else								i = htobe(i);
	
	mem->bytes.insert((vector<unsigned char>::iterator)tellData(), *((const unsigned char*)&i), 
					  *((const unsigned char*)&i+sizeof(i)));
	
	
	this->seek(sizeof(i), OO::cur);
}

void OByteArray::write(int64_t i) {
	//check if we need to swap the byte ordering
	if(mem->end == OO::LittleEndian)	i = htole(i);
	else								i = htobe(i);
	
	mem->bytes.insert((vector<unsigned char>::iterator)tellData(), *((const unsigned char*)&i), 
					  *((const unsigned char*)&i+sizeof(i)));
	
	
	this->seek(sizeof(i), OO::cur);
}

void OByteArray::write(uint8_t i) {
	//check if we need to swap the byte ordering
	if(mem->end == OO::LittleEndian)	i = htole(i);
	else								i = htobe(i);
	
	mem->bytes.insert((vector<unsigned char>::iterator)tellData(), *((const unsigned char*)&i), 
					  *((const unsigned char*)&i+sizeof(i)));
	
	
	this->seek(sizeof(i), OO::cur);
}

void OByteArray::write(uint16_t i) {
	//check if we need to swap the byte ordering
	if(mem->end == OO::LittleEndian)	i = htole(i);
	else								i = htobe(i);
	
	mem->bytes.insert((vector<unsigned char>::iterator)tellData(), *((const unsigned char*)&i), 
					  *((const unsigned char*)&i+sizeof(i)));
	
	
	this->seek(sizeof(i), OO::cur);
}

void OByteArray::write(uint32_t i) {
	//check if we need to swap the byte ordering
	if(mem->end == OO::LittleEndian)	i = htole(i);
	else								i = htobe(i);
	
	mem->bytes.insert((vector<unsigned char>::iterator)tellData(), *((const unsigned char*)&i), 
					  *((const unsigned char*)&i+sizeof(i)));
	
	
	this->seek(sizeof(i), OO::cur);
}

void OByteArray::write(uint64_t i) {
	//check if we need to swap the byte ordering
	if(mem->end == OO::LittleEndian)	i = htole(i);
	else								i = htobe(i);
	
	mem->bytes.insert((vector<unsigned char>::iterator)tellData(), *((const unsigned char*)&i), 
					  *((const unsigned char*)&i+sizeof(i)));
	
	
	this->seek(sizeof(i), OO::cur);
}

OByteArray& OByteArray::operator<<(OByteArray& ba) {
	write(ba);
	return *this;
}

OByteArray& OByteArray::operator<<(OByteArray&& ba) {
	write(ba);
	return *this;
}

OByteArray& OByteArray::operator<<(const OString& str) {
	write(str);
	return *this;
}

OByteArray& OByteArray::operator<<(const OString&& str) {
	write(str);
	return *this;
}

OByteArray& OByteArray::operator<<(string str) {
	write(str);
	return *this;
}

OByteArray& OByteArray::operator<<(const char* str) {
	write(str);
	return *this;
}

OByteArray& OByteArray::operator<<(OSerializable& obj) {
	write(obj);
	return *this;
}

OByteArray& OByteArray::operator<<(bool i) {
	write(i);
	return *this;
}

OByteArray& OByteArray::operator<<(int8_t i) {
	write(i);
	return *this;
}

OByteArray& OByteArray::operator<<(int16_t i) {
	write(i);
	return *this;
}

OByteArray& OByteArray::operator<<(int32_t i) {
	write(i);
	return *this;
}

OByteArray& OByteArray::operator<<(int64_t i) {
	write(i);
	return *this;
}

OByteArray& OByteArray::operator<<(uint8_t i) {
	write(i);
	return *this;
}

OByteArray& OByteArray::operator<<(uint16_t i) {
	write(i);
	return *this;
}

OByteArray& OByteArray::operator<<(uint32_t i) {
	write(i);
	return *this;
}

OByteArray& OByteArray::operator<<(uint64_t i) {
	write(i);
	return *this;
}

void OByteArray::append(OByteArray& ba) {
	register int pos = tell();
	this->seek(0, OO::end);
	write(ba);
	this->seek(pos);
}

void OByteArray::append(OByteArray&& ba) {
	register int pos = tell();
	this->seek(0, OO::end);
	write(ba);
	this->seek(pos);
}

void OByteArray::append(const OString& str) {
	register int pos = tell();
	this->seek(0, OO::end);
	write(str);
	this->seek(pos);
}

void OByteArray::append(const OString&& str) {
	register int pos = tell();
	this->seek(0, OO::end);
	write(str);
	this->seek(pos);
}

void OByteArray::append(const string str) {
	register int pos = tell();
	write(str);
	int seek(pos);
}

void OByteArray::append(const char* str) {
	register int pos = tell();
	write(str);
	int seek(pos);
}

void OByteArray::append(const char* str, int len) {
	register int pos = tell();
	write(str, len);
	int seek(pos);
}

void OByteArray::append(OSerializable& obj) {
	makeOwner();
	
	mem->dir = OO::Input;
	obj.serialize(*this, mem->dir);
}

void OByteArray::append(bool i) {
	mem->bytes.insert((vector<unsigned char>::iterator)end(), *((const unsigned char*)&i), 
					  *((const unsigned char*)&i+sizeof(i)));
	
	
	this->seek(sizeof(i), OO::cur);
}

void OByteArray::append(int8_t i) {
	mem->bytes.insert((vector<unsigned char>::iterator)end(), *((const unsigned char*)&i), 
					  *((const unsigned char*)&i+sizeof(i)));
	
	
	this->seek(sizeof(i), OO::cur);
}

void OByteArray::append(int16_t i) {
	//check if we need to swap the byte ordering
	if(mem->end == OO::LittleEndian)	i = htole(i);
	else								i = htobe(i);
	
	mem->bytes.insert((vector<unsigned char>::iterator)end(), *((const unsigned char*)&i), 
					  *((const unsigned char*)&i+sizeof(i)));
	
	
	this->seek(sizeof(i), OO::cur);
}

void OByteArray::append(int32_t i) {
	//check if we need to swap the byte ordering
	if(mem->end == OO::LittleEndian)	i = htole(i);
	else								i = htobe(i);
	
	mem->bytes.insert((vector<unsigned char>::iterator)end(), *((const unsigned char*)&i), 
					  *((const unsigned char*)&i+sizeof(i)));
	
	
	this->seek(sizeof(i), OO::cur);
}

void OByteArray::append(int64_t i) {
	//check if we need to swap the byte ordering
	if(mem->end == OO::LittleEndian)	i = htole(i);
	else								i = htobe(i);
	
	mem->bytes.insert((vector<unsigned char>::iterator)end(), *((const unsigned char*)&i), 
					  *((const unsigned char*)&i+sizeof(i)));
	
	
	this->seek(sizeof(i), OO::cur);
}

void OByteArray::append(uint8_t i) {
	mem->bytes.insert((vector<unsigned char>::iterator)end(), *((const unsigned char*)&i), 
					  *((const unsigned char*)&i+sizeof(i)));
	
	
	this->seek(sizeof(i), OO::cur);
}

void OByteArray::append(uint16_t i) {
	//check if we need to swap the byte ordering
	if(mem->end == OO::LittleEndian)	i = htole(i);
	else								i = htobe(i);
	
	mem->bytes.insert((vector<unsigned char>::iterator)end(), *((const unsigned char*)&i), 
					  *((const unsigned char*)&i+sizeof(i)));
	
	
	this->seek(sizeof(i), OO::cur);
}

void OByteArray::append(uint32_t i) {
	//check if we need to swap the byte ordering
	if(mem->end == OO::LittleEndian)	i = htole(i);
	else								i = htobe(i);
	
	mem->bytes.insert((vector<unsigned char>::iterator)end(), *((const unsigned char*)&i), 
					  *((const unsigned char*)&i+sizeof(i)));
	
	
	this->seek(sizeof(i), OO::cur);
}

void OByteArray::append(uint64_t i) {
	//check if we need to swap the byte ordering
	if(mem->end == OO::LittleEndian)	i = htole(i);
	else								i = htobe(i);
	
	mem->bytes.insert((vector<unsigned char>::iterator)end(), *((const unsigned char*)&i), 
					  *((const unsigned char*)&i+sizeof(i)));
	
	
	this->seek(sizeof(i), OO::cur);
}

void OByteArray::read(bool& i) {
	bool* tmpptr = (bool*)constTellData();
	streamptr += sizeof(i);
	i = *tmpptr;
}

void OByteArray::read(char &i) {
	char* tmpptr = (char*)constTellData();
	streamptr += sizeof(i);
	i = *tmpptr;
}

void OByteArray::read(int8_t& i) {
	int8_t* tmpptr = (int8_t*)constTellData();
	streamptr += sizeof(i);
	i = *tmpptr;
}

void OByteArray::read(int16_t& i) {
	int16_t* tmpptr = (int16_t*)constTellData();
	streamptr += sizeof(i);
	
	//check if we need to swap the byte ordering
	if(mem->end == OO::LittleEndian)	i = letoh(*tmpptr);
	else								i = betoh(*tmpptr);
}

void OByteArray::read(int32_t& i) {
	int32_t* tmpptr = (int32_t*)constTellData();
	streamptr += sizeof(i);
	
	//check if we need to swap the byte ordering
	if(mem->end == OO::LittleEndian)	i = letoh(*tmpptr);
	else								i = betoh(*tmpptr);
}

void OByteArray::read(int64_t& i) {
	int64_t* tmpptr = (int64_t*)constTellData();
	streamptr += sizeof(i);
	
	//check if we need to swap the byte ordering
	if(mem->end == OO::LittleEndian)	i = letoh(*tmpptr);
	else								i = betoh(*tmpptr);
}

void OByteArray::read(uint8_t& i) {
	uint8_t* tmpptr = (uint8_t*)constTellData();
	streamptr += sizeof(i);
	i = *tmpptr;
}

void OByteArray::read(uint16_t& i) {
	uint16_t* tmpptr = (uint16_t*)constTellData();
	streamptr += sizeof(i);
	
	//check if we need to swap the byte ordering
	if(mem->end == OO::LittleEndian)	i = letoh(*tmpptr);
	else								i = betoh(*tmpptr);
}

void OByteArray::read(uint32_t& i) {
	uint32_t* tmpptr = (uint32_t*)constTellData();
	streamptr += sizeof(i);
	
	//check if we need to swap the byte ordering
	if(mem->end == OO::LittleEndian)	i = letoh(*tmpptr);
	else								i = betoh(*tmpptr);
}

void OByteArray::read(uint64_t& i) {
	uint64_t* tmpptr = (uint64_t*)constTellData();
	streamptr += sizeof(i);
	
	//check if we need to swap the byte ordering
	if(mem->end == OO::LittleEndian)	i = letoh(*tmpptr);
	else								i = betoh(*tmpptr);
}

void OByteArray::read(OSerializable& obj) {
	mem->dir = OO::Output;
	
	obj.serialize(*this, mem->dir);
}

void OByteArray::read(OString& str) {
	str =  OString((const char*)tellData());
	
	int len = ::strlen((const char*)tellData());
	seek(len+1, OO::cur);
}

#ifdef OO_BOTAN
OByteArray OByteArray::encrypt(Botan::Public_Key* key) {
	
}

OByteArray OByteArray::decrypt(Botan::Public_Key* key) {
	
}
#endif

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
	
	advanceSize(length);
	
	//read in the data
	file.read((char*)data(), length);
	
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
	file.write((char*)this->data(), this->size());
	
	//we are done here so close the file
	file.close();
	
	return true;
}

OList<OByteArray> OByteArray::chunkData(const void *data, int dsize, int size) {
	return chunkDataWithHeader(data, dsize, size, NULL);
}

OList<OByteArray> OByteArray::chunkDataWithHeader(const void *data, int dsize, int size, 
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
		array.write(data+i*size, size);
		bytes.push_back(array);
	}
	
	//write on the remainder
	if(remainder) {
		OByteArray array;
		if(cbk) cbk(array, chunks, remainder);
		array.write(data+chunks*size, remainder);
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
		array.advanceSize(size);
		file.read((char*)array.tellData(), size);
		bytes.push_back(array);
	}
	
	//write the remainder
	if(remainder) {
		OByteArray array;
		if(cbk) cbk(array, chunks, remainder);
		array.resize(length);
		array.advanceSize(remainder);
		file.read((char*)array.tellData(), remainder);
		bytes.push_back(array);
	}
	
	return bytes;
}

int OByteArray::find(const OByteArray &data, int start) {
	return find(data.constData(), data.size(), start);
}

int OByteArray::find(const void *data, int length, int start) {
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
	::memcpy(ptr, constTellData(), readlen);
	this->seek(readlen, OO::cur);
	return readlen;
}

int OByteArray::checksum() const {
	int length = size();
	register int sum = 0;
	register unsigned char* ptr = (unsigned char*)begin();
	for(register int i=0; i<length; i++) sum += ptr[i];
	return sum;
}

int OByteArray::tell() const {
	return streamptr;
}

void OByteArray::seek(int pos, OO::IOBase base) {
	(this->*seekSwitch[base])(pos);
	
	if(streamptr > size()) streamptr = size();
	else if(streamptr < 0) streamptr = 0;
}

unsigned OByteArray::size() const {
	return mem->bytes.size();
}

void OByteArray::seekCurrent(int len) {
	streamptr += len;
}

void OByteArray::seekBegin(int len) {
	streamptr = len;
}

void OByteArray::seekEnd(int len) {
	streamptr = size() + len;
}


void OByteArray::advanceSize(int addition) {
	makeOwner();
	unsigned char tmp = 0;
	mem->bytes.insert((vector<unsigned char>::iterator)begin(), addition, tmp);
}

/*
void OByteArray::advanceSizePrepend(int addition) {
	mem->sizeofprepend -= addition;
	mem->sizeofdata += addition;
	mem->sizeofarray += addition;
	seek(addition, OO::cur);
}
*/

OByteArray::Iterator OByteArray::tellData() {
	return begin() + streamptr;
}

OByteArray::ConstIterator OByteArray::tellData() const {
	return constTellData();
}

OByteArray::ConstIterator OByteArray::constTellData() const {
	return begin() + streamptr;
}

unsigned OByteArray::dataLeft() const {
	int ret = mem->bytes.size() - streamptr;
	return ret < 0 ? 0 : ret;
}

void OByteArray::resize(int len) {
	makeOwner();
	mem->bytes.resize(len);
}

void OByteArray::enlarge(int len) {
	makeOwner();
	mem->bytes.resize(mem->bytes.capacity()+len);
}

unsigned char* OByteArray::data() {
	makeOwner();
	return begin();
}

const unsigned char* OByteArray::constData() const {
	return begin();
}

void OByteArray::clear() {
	mem->bytes.clear();
	streamptr = 0;
}

OO::Endian OByteArray::endian() const {
	return mem->end;
}

void OByteArray::setEndian(OO::Endian end) {
	makeOwner();
	mem->end = end;
}

OByteArray::Iterator OByteArray::end() {
	makeOwner();
	return reinterpret_cast<Iterator>(&mem->bytes[mem->bytes.size()]);
}

OByteArray::Iterator OByteArray::begin() {
	makeOwner();
	return reinterpret_cast<Iterator>(&mem->bytes[0]);
}

OByteArray::ConstIterator OByteArray::end() const {
	return reinterpret_cast<Iterator>(&mem->bytes[mem->bytes.size()]);
}

OByteArray::ConstIterator OByteArray::begin() const {
	return reinterpret_cast<Iterator>(&mem->bytes[0]);
}

void OByteArray::makeOwner() {
	if(mem.use_count() > 1) {
		OByteArray tmp(size());
		tmp.write(this->begin(), size());
		tmp.seek(streamptr);
		tmp.mem->dir = this->mem->dir;
		tmp.mem->end = this->mem->end;
		*this = tmp;
	}
}

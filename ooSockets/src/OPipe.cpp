#include"OPipe.hpp"

OPipe::OPipe(OThread *parent) : OIODevice() {
	//create the pipe
#ifdef __windows__
	CreatePipe(&fdes[0], &fdes[1], 0, 0);
#else
	::pipe(fdes);
#endif
	par = parent;
	
	registerFd();
}

OPipe::OPipe(const OPipe &other, OThread *parent) : OIODevice() {
	fdes[0] = other.fdes[0];
	fdes[1] = other.fdes[1];
	
	//we only want to copy over the file descriptors
	par = parent;
	registerFd();
}

OPipe::~OPipe() {
	//make sure the read fd gets removed from the thread properly
	unregisterFd();
	
	//set the parent pointer to 0 for safety
	//to make sure the parent doesn't accidentally
	//get free'd
	par = 0;
	
	//close my end of the pipes
#ifdef __windows__
	CloseHandle(fdes[0]);
	CloseHandle(fdes[1]);
#else
	::close(fdes[0]);
	::close(fdes[1]);
#endif
}

void OPipe::operator=(const OPipe& other) {
	fdes[0] = other.fdes[0];
	fdes[1] = other.fdes[1];
}

void OPipe::write(OByteArray &data, int len) {
#ifdef __windows__
	DWORD written = 0;
	if(!WriteFile(fdes[1], data.data(), len, &written, NULL)) {
		if(written <= 0) {
			sigError();
		}
	}
#else
	if(::write(fdes[1], data.data(), len) < 0) {
		sigError();
	}
#endif
}

void OPipe::write(OByteArray &data) {
#ifdef __windows__
	DWORD written = 0;
	if(!WriteFile(fdes[1], data.data(), data.size(), &written, NULL)) {
		if(written <= 0) {
			sigError();
		}
	}
#else
	write(data, data.size());
#endif
}

OByteArray OPipe::read(int len) {
	OByteArray data;
	data.resize(len);

#ifdef __windows__
	DWORD read = 0;
	if(!ReadFile(fdes[0], data.data(), len, &read, NULL)) {
		return data;
	}
	data.advanceSize(read);
#else
	int ret;
	if((ret = ::read(fdes[0], data.data(), len)) > 0) {
		data.advanceSize(len);
		return data;
	} else {
		sigError();
	}
#endif

	return OByteArray();
}

OByteArray OPipe::readAll() {
	return read(available());
}

void OPipe::registerFd() {
    if(par) {
		par->registerReadFD(readFd(), this);
    }
}

void OPipe::unregisterFd() {
	if(par) {
		par->unregisterReadFD(readFd());
	}
}

void OPipe::parent(OThread *p) {
	unregisterFd();
	
	par = p;
	registerFd();
}

int OPipe::available() {
#ifdef __windows__
	DWORD sum = 0;
	if(!PeekNamedPipe(fdes[0], NULL, NULL, NULL, &sum, NULL)) {
		sigError();
		return 0;
	}
#else
	int sum = 0;
	//an error was generated so handle it
	if(ioctl(fdes[0], FIONREAD, &sum) < 0) {
		sigError();
		return 0;
	}
#endif

	return sum;
}

void OPipe::readFunc(function<void ()> cbk) {
	readCbk = cbk;
}

void OPipe::errorFunc(function<void (OPipeError)> cbk) {
	errorCbk = cbk;
}

void OPipe::readLoop() {
	sigRead();
}

void OPipe::writeLoop() {
	
}

void OPipe::errorLoop() {
	
}

void OPipe::priorityLoop() {
	
}

OO::HANDLE OPipe::fileDescriptor() const {
    return readFd();
}

OO::HANDLE OPipe::readFd() const {
	return fdes[0];
}

void OPipe::readFd(OO::HANDLE fd) {
	fdes[0] = fd;
}

OO::HANDLE OPipe::writeFd() const {
	return fdes[1];
}

void OPipe::writeFd(OO::HANDLE fd) {
	fdes[1] = fd;
}

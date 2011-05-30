#include "OSerial.hpp"

//**************************************************
//OPort
//**************************************************
OPort::OPort() {
	p_index = 0;
	p_type = OO::NullPort;
}

OPort::OPort(unsigned int index, OO::SerialType type) {
#ifdef __linux__
	if(type == OO::UsbPort) {
		p_name<<"UsbOSerial" <<index;
		p_device<<"/dev/ttyUSB" <<index;
	} else if(OO::HwPort) {
		p_name<<"OSerial" <<index;
		p_device<<"/dev/ttyS" <<index;
	} else {
		p_index = 0;
		p_type = OO::NullPort;
	}
#elif defined(__apple)
	OPortList list = OSerial::portList();
	if(index < list.size())
		this->operator=(list[index]);
	else {
		p_index = 0;
		p_type = OO::NullPort;
	}
#elif defined(__windows__)

#endif
}

OString OPort::name() const {
	return p_name;
}

OString OPort::hwName() const {
	return p_device;
}

OO::SerialType OPort::type() const {
	return p_type;
}

bool OPort::isEmpty() const {
	return type() == OO::NullPort;
}

//**************************************************
//OSerial
//**************************************************
OSerial::OSerial(OThread* parent) {
	
	par = parent;
	
	serialerr = 0;
	fdes = 0;
	
	//setup the default options for the serial port
#ifdef __windows__
	
#else
	bzero(&term, sizeof(term));
	
	term.c_cflag = OO::Char8 | CREAD | CLOCAL;
	
	term.c_cc[VMIN]     = 1;     // blocking read until 1 character arrives 
	
#endif
}

OSerial::~OSerial() {
	unregisterFD();
}

OPortList OSerial::portList() {
	OPortList ret;
	
	OStringList availports;
	
	fs::path files("/dev");
	fs::directory_iterator begin(files);
	fs::directory_iterator end;
	
	/*
	for_each(begin, end,
			 [&availports] (fs::directory_entry& p) -> void {
		OString path = p.path().string();
		path = path.substring(path.find_last_of("/")+1);
		availports.push_back(path);
	});
	*/
	
	for(fs::directory_iterator it=begin; it!=end; it++) {
		OString path = it->path().string();
		path = path.substring(path.find_last_of("/")+1);
		availports.push_back(path);
	}
	
#ifdef __linux__
	boost::regex hwports("ttyS([0-9]+)");
	boost::regex usbports("ttyUSB([0-9]+)");
	
	unsigned length = availports.size();
	for(unsigned i=0; i<length; i++) {
		boost::cmatch cap;
		
		//if this is a hw port set it us as such
		if(boost::regex_match(availports[i].toCString(), cap, 
							  hwports)) {
			OPort tmp;
			tmp.p_type = OO::HwPort;
			tmp.p_index = atoi(cap[1].first);
			tmp.p_name<<"Serial" <<tmp.p_index;
			tmp.p_device<<"/dev/" <<availports[i];
			ret.push_back(tmp);
		} else if(boost::regex_match(availports[i].toCString(), 
									 cap, usbports)) {
			OPort tmp;
			tmp.p_type = OO::UsbPort;
			tmp.p_index = atoi(cap[0].first);
			tmp.p_name<<"UsbSerial" <<tmp.p_index;
			tmp.p_device<<"/dev/" <<availports[i];
			ret.push_back(tmp);
		}
	}
	
#endif
	
#ifdef __apple__
	int index = 0;
	unsigned length = availports.size();
	for(unsigned i=0; i<length; i++) {
		boost::cmatch cap;
		if(availports[i].startsWith("tty.")) {
			OPort tmp;
			tmp.p_index = index++;
			if(availports[i].contains("usbserial") || availports[i].contains("usb")) {
				tmp.p_name<<"UsbSerial" <<tmp.p_index;
				tmp.p_type = OO::UsbPort;
			} else {
				tmp.p_name<<"Serial" <<tmp.p_index;
				tmp.p_type = OO::HwPort;
			}
			tmp.p_device<<"/dev/" <<availports[i];
			ret.push_back(tmp);
		}
	}
	
	
#endif
	
	return ret;
}

void OSerial::open(const OPort &port) {
	errno = 0;
	
	i_port = port;
	
	OPort tmp;
	//if the user opted to used the first available port
	//then get the list of ports
	if(port.isEmpty()) {
		OPortList list = OSerial::portList();
		if(list.size() > 0)
			tmp = list[0];
		else {
			//TODO
			//create an error code and return it, to signal that
			//there were no available serial ports to connect to
			//by default
			
			return;
		}
	} else
		tmp = port;
	
#ifdef __windows__
	wstring tmpstr(tmp.p_device.length(), L'');
	copy(tmp.p_device.begin(), tmp.p_device.end(), tmpstr.begin());
	fdes = CreateFile(tmpstr.c_str(), GENERIC_READ | GENERIC_WRITE, 
		0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
#else
	fdes = ::open(tmp.p_device.toCString(), O_RDWR | O_NONBLOCK | O_NOCTTY);
#endif
	
#ifdef __windows__
	if(fdes == INVALID_HANDLE_VALUE) {
#else
	if(fdes == -1) {
#endif
		sigError();
		return;
	}
	
#ifdef __windows__
	DCB serparam = {0};

	if(!GetCommState(fdes, &serparam)) {
		sigError();
		return;
	}

	serparam.BaudRate = speed;
	serparam.ByteSize = 8;
	serparam.StopBits = ONESTOPBIT;
	serparam.Parity = NOPARITY;

	if(!SetCommState(fdes, &serparam)) {
		sigError();
		return;
	}


#else
	
	if(::tcsetattr(fdes, TCSANOW, &term) == -1) {
		sigError();
		return;
	}
#endif
	
	registerFD();
	
	serialerr = 0;
}

OByteArray OSerial::read(int len) {
	OByteArray ba;
	
	if(len <= 0) {
		return ba;
	}
	
	//make sure the container can fit all the data
	//we are about to read
	ba.resize(len);
#ifdef __windows__
	DWORD readlen = 0;
	ReadFile(fdes, ba.data(), len, &readlen, NULL);
#else
	int recvlen = ::read(fdes, ba.data(), len);
#endif
	
	if(recvlen <= 0) {
		ba.resize(ba.size() - len);
		if(recvlen < 0) sigError();
	} else {
		ba.resize(ba.size() - (len - recvlen));
	}
	
	return ba;
}

bool OSerial::isOpen() const {
	return fdes > 0;
}

void OSerial::close() {
	::close(fdes);
}

OO::SerialSpeed OSerial::speed() const {
#ifdef _windows_
	
#else
	return (OO::SerialSpeed)cfgetospeed(&term);
#endif
}

void OSerial::setSpeed(OO::SerialSpeed opt) {
#ifdef _windows_
	
#else
	//set the speed
	cfsetospeed(&term, opt);
	cfsetispeed(&term, opt);
	
	//update the port
	if(fdes)
		tcsetattr(fdes, TCSANOW, &term);
#endif
}

OO::SerialCS OSerial::charSize() const {
#ifdef _windows_
	
#else
	return OO::SerialCS(CSIZE & term.c_cflag);
#endif
}

void OSerial::setCharSize(OO::SerialCS size) {
#ifdef _windows_
	
#else	
	term.c_cflag &= ~CSIZE;//clear the current speed
	term.c_cflag |= size;//set the new speed
	
	//update the port
	if(fdes)
		tcsetattr(fdes, TCSANOW, &term);
#endif
}

int OSerial::stopBits() const {
#ifdef _windows_
	
#else
	if(term.c_cflag & CSTOPB)
		return 2;
	
	return 1;
#endif
}

void OSerial::setStopBits(int n) {
#ifdef _windows_
	
#else
	if(n == 2)
		term.c_cflag |= CSTOPB;//2 stop bits
	else
		term.c_cflag &= ~CSTOPB;//1 stop bit
	
	if(fdes)
		tcsetattr(fdes, TCSANOW, &term);
#endif
}

OO::SerialParity OSerial::parity() const {
#ifdef _windows_
	
#else
	if(term.c_cflag & PARENB) {
		return OO::SerialParity(term.c_cflag & OO::Odd);
	}
	
	//parity set to off
	return OO::NoParity;
#endif
}

void OSerial::setParity(OO::SerialParity p) {
#ifdef _windows_
	
#else
	switch(p) {
		case OO::Even: {
			term.c_cflag |= PARENB;
			term.c_cflag &= ~OO::Odd;
			break;
		}
		case OO::Odd: {
			term.c_cflag |= PARENB;
			term.c_cflag |= OO::Odd;
			break;
		}
		case OO::NoParity: {
			term.c_cflag &= ~PARENB;
			break;
		}
	}
	
	if(fdes)
		tcsetattr(fdes, TCSANOW, &term);
#endif
}

OO::SerialFlow OSerial::flowControl() const {
#ifdef _windows_
	
#else
	if(term.c_iflag & IXON) return OO::HardwareFlow;
	
	return OO::NoFlow;
#endif
}

void OSerial::setFlowControl(OO::SerialFlow fc) {
#ifdef _windows_
	
#else
	if(fc == OO::HardwareFlow) {
		term.c_iflag |= IXON;
		term.c_iflag |= IXOFF;
	} else {
		term.c_iflag &= ~IXON;
		term.c_iflag &= ~IXOFF;
	}
	
	if(fdes)
		tcsetattr(fdes, TCSANOW, &term);
#endif
}

OByteArray OSerial::readAll() {
	return read(available());
}

OPort OSerial::port() {
	return i_port;
}

void OSerial::write(OByteArray& data) {
	if(data.size() <= 0) {
		return;
	}
	
	//make sure the container can fit all the data
	//we are about to read
#ifdef __windows__
	DWORD writelen = 0;
	WriteFile(fdes, data.data(), data.size(), &writelen, NULL);
#else
	int writelen = ::write(fdes, data.data(), data.size());
#endif
	
	if(writelen < 0) sigError();
}

OThread* OSerial::parent() {
	return par;
}

void OSerial::parent(OThread* parent) {
	unregisterFD();
	par = parent;
	registerFD();
}

int OSerial::error() {
	return serialerr.code();
}

OString OSerial::strError() {
	return serialerr.string();
}

void OSerial::readFunc(function<void ()> cbk) {
	readyReadCbk = cbk;
}

void OSerial::errorFunc(function<void (OSerialError)> cbk) {
	errorCbk = cbk;
}

int OSerial::available() {
	int sum = 0;
#ifdef __windows__
	int back = 0;
#else
	int back = ::ioctl(fdes, FIONREAD, &sum);
#endif
	
	//an error was generated so handle it
	if(back < 0) {
		sigError();
		return 0;
	}
	
	return sum;
}

void OSerial::waitForRead(int msec) {
	timeval tmpt;
	tmpt.tv_sec = msec / 1000;
	tmpt.tv_usec = (msec % 1000) * 1000;
	
	fd_set tmpset;
	FD_ZERO(&tmpset);
	FD_SET((OO::SOCKET)fdes, &tmpset);
	
#ifdef __windows__
	::select(0, &tmpset, 0, 0, &tmpt);
#else
	::select(fdes + 1, &tmpset, 0, 0, &tmpt);
#endif

	readLoop();
}

OSerial& OSerial::operator =(OSerial& other) {
	//copy over all the variables
	fdes = other.fdes;
	par = other.par;
	
	//don't forget the function callbacks
	this->readyReadCbk = other.readyReadCbk;
	this->errorCbk = other.errorCbk;
	
	//unregister the pointer to the other object and register
	//this on in it's place
	other.unregisterFD();
	this->registerFD();
	
	//have to clear the old serial object to make sure
	//it does not mess with the file descriptor
	other.fdes = NULL;
	other.par = NULL;
	other.readyReadCbk = NULL;
	other.errorCbk = NULL;

	return *this;
}

OO::HANDLE OSerial::handle() const {
	return fdes;
}

void OSerial::errorLoop() {
	if(error()) {
		sigError();
	}
}

void OSerial::writeLoop() {
	
}

void OSerial::readLoop() {
	if(available()) {
		sigReadyRead();
	}
}

void OSerial::priorityLoop() {
	
}

void OSerial::registerFD() {
	if(par) {
		par->registerReadFD(fdes, this);
	}
}

void OSerial::unregisterFD() {
	if(par) {
		par->unregisterReadFD(fdes);
	}
}

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
	fdreg = false;
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
	
	for_each(begin, end,
			 [&availports] (fs::directory_entry& p) -> void {
		OString path = p.path().string();
		path = path.substring(path.find_last_of("/")+1);
		availports.push_back(path);
	});
	
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

void OSerial::open(OO::SerialSpeed speed, 
				   const OPort &port, 
				   int opts) {
	errno = 0;
	
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
	struct termios settings;
	
	if(::tcgetattr(fdes, &settings) == -1) {
		sigError();
		return;
	}
	
	::memset(&settings, 0, sizeof(settings));
	
//	settings.c_cflag = speed /*| CRTSCTS */| CS8 | CLOCAL | CREAD;
//	settings.c_iflag = IGNPAR;// | IGNCR;// | IXOFF | ICRNL;
//	settings.c_oflag = 0;
//	settings.c_lflag = 0;//ICANON;
	
	//arduino settings
	//	settings.c_iflag =  |  |  |  |  |  |  |  | 
//		 |  |  | ECHOCTL | ECHOKE |  | IXON;
	
	settings.c_cflag = CS8 | CREAD | CLOCAL | (int)speed;
	settings.c_iflag = 0;//IGNBRK | BRKINT | ICRNL | IXON;
	settings.c_oflag = 0;//OPOST | ONLCR;
	settings.c_lflag = 0;//ISIG | ICANON | IEXTEN | NOFLSH;// | ECHO | ECHOE | ECHOK;
	
	settings.c_cc[VINTR]    = 0;     /* Ctrl-c */ 
	settings.c_cc[VQUIT]    = 0;     /* Ctrl-\ */
	settings.c_cc[VERASE]   = 0;     /* del */
	settings.c_cc[VKILL]    = 0;     /* @ */
	settings.c_cc[VEOF]     = 0;     /* Ctrl-d */
	settings.c_cc[VTIME]    = 0;     /* inter-character timer unused */
	settings.c_cc[VMIN]     = 1;     /* blocking read until 1 character arrives */
#ifndef __apple__
	settings.c_cc[VSWTC]    = 0;     /* '\0' */
#endif
	settings.c_cc[VSTART]   = 0;     /* Ctrl-q */ 
	settings.c_cc[VSTOP]    = 0;     /* Ctrl-s */
	settings.c_cc[VSUSP]    = 0;     /* Ctrl-z */
	settings.c_cc[VEOL]     = 0;     /* '\0' */
	settings.c_cc[VREPRINT] = 0;     /* Ctrl-r */
	settings.c_cc[VDISCARD] = 0;     /* Ctrl-u */
	settings.c_cc[VWERASE]  = 0;     /* Ctrl-w */
	settings.c_cc[VLNEXT]   = 0;     /* Ctrl-v */
	settings.c_cc[VEOL2]    = 0;     /* '\0' */
	
	if(::tcsetattr(fdes, TCSANOW, &settings) == -1) {
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
	int readlen = ::read(fdes, ba.data(), len);
#endif
	if(readlen > 0) {
		ba.advanceSize(readlen);
	} else if(readlen < 0) {
		sigError();
	}
	
	return ba;
}

OByteArray OSerial::readAll() {
	return read(available());
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

void OSerial::readyReadFunc(function<void ()> cbk) {
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

void OSerial::waitForReadyRead(int msec) {
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
	fdreg = other.fdreg;
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
	other.fdreg = false;
	other.fdes = NULL;
	other.par = NULL;
	other.readyReadCbk = NULL;
	other.errorCbk = NULL;

	return *this;
}

OO::HANDLE OSerial::fileDescriptor() const {
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
	if(!fdreg && par) {
		par->registerReadFD(fdes, this);
		fdreg = true;
	}
}

void OSerial::unregisterFD() {
	if(fdreg) {
		par->unregisterReadFD(fdes);
		fdreg = false;
	}
}

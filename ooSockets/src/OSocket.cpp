#include"OSocket.hpp"

bool OSocket::initSockLimit = false;

OSocket::OSocket(OThread *parent) {
	//set the socket limit to a much larger number
	if(!initSockLimit) {
#ifdef __apple__
		::system("ulimit -n `sysctl -n kern.maxfilesperproc`");
#elif defined(__linux)
	
#elif defined(__windows__)
	
#endif
		initSockLimit = true;
	}


	_clearError();
	qtpar = 0;
	par = parent;
	sockerr = 0;
	conn = false;
	fdes = 0;
	esigstat = true;
	writestat = false;
}

#ifdef OO_QT
OSocket::OSocket(QObject *parent) {
	//set the socket limit to a much larger number
	if(!initSockLimit) {
#ifdef __apple__
		::system("ulimit -n `sysctl -n kern.maxfilesperproc`");
#elif defined(__linux)
	
#elif defined(__windows__)
	
#endif
		initSockLimit = true;
	}
	
	_clearError();
	qtpar = parent;
	par = 0;
	sockerr = 0;
	conn = false;
	fdes = 0;
	esigstat = true;
	writestat = false;
	
}

#endif

OSocket::~OSocket() {
	if(connected()) {
		close();
	}
	
	par = 0;
	
}

void OSocket::close() {
	if(fdes) {
		shutdown(fdes, SHUT_RDWR);
		_close(fdes);
//		unregisterFD();
		disableReadyWrite();
		fdes = 0;
		conn = false;
		_clearError();
	}
}

bool OSocket::readyWriteEnabled() const {
#ifdef OO_QT
	if(qt_read.get()) {
		return writestat || qt_read->isEnabled();
	}
	return writestat;
#else
	return writestat;
#endif
}

void OSocket::enableReadyWrite() {
#ifdef OO_QT
	if(qt_write.get()) {
		qt_write->setEnabled(true);
	} else if(fdes && qtpar) {
		qt_write.reset(new QSocketNotifier(fdes, QSocketNotifier::Write, qtpar));
		QObject::connect(qt_write.get(), SIGNAL(activated(int)), this, 
						 SLOT(readyWriteSlot(int)));
	}
#endif
	if(readyWriteCbk && !writestat && fdes) {
		par->registerWriteFD((OO::HANDLE)fdes, this);
		writestat = true;
	}
}

void OSocket::disableReadyWrite() {
#ifdef OO_QT
	if(qt_write.get()) {
		qt_write->setEnabled(false);
	}
#endif
	if(writestat) {
		par->unregisterWriteFD((OO::HANDLE)fdes);
		writestat = false;
	}
}

bool OSocket::connect(OString addr, unsigned short port, 
					  OO::SockType type, OO::SockFamily family) {
	_clearError();
	
	int callerr = 0;
	
	OAddressList list = OSocket::getAddressInfo(addr, port, type, 
												family, OO::NullFlag, &callerr);
	
	//if an error was generated during the call to getAddressInfo then emit the
	//error signal
	if(callerr) {
		sigError(callerr);
		return false;
	}
	
	//if no OAddressInfo's were return then just return
	if(!list.size()) {
		return false;
	}
	
	//disable the error signal while calling OSocket::connect so a ton of error
	//signals are not passed to the user
	disableErrorSig();
	
	//connect to the first OAddressInfo that was returned
	for(unsigned i=0; i<list.size(); i++) {
		if(!connect(list[i])) {
			continue;
		}
	}
	
	//now that we are done connecting to a socket enable error signals
	enableErrorSig();
	
	//if an error was generated report it
	if(error())
		sigError();
	
	return true;
}

bool OSocket::connect(const OSockAddress &addr, OO::SockType type) {
	OAddressInfo info;
	info.address(addr);
	info.socktype(type);
	info.family(addr.family());
	
	return connect(info);
}

bool OSocket::connect(const OAddressInfo &info) {
	//make sure this socket class is not already maintaining a socket
	close();
	
	//clear any set error before we attempt to make any connections
	_clearError();
	
	//create a socket and on error return false
	if((fdes = ::socket(info.family(), info.socktype(), info.protocol())) == -1) {
		sigError();
		fdes = 0;
		return false;
	}
	
	//only for Tcp connections do we need to call ::connect()
	if(info.socktype() == OO::Tcp) {
		OSockAddress addr(info.address());
		if(::connect(fdes, addr.constData(), addr.size()) == -1) {
			sigError();
			_close(fdes);
			fdes = 0;
			return false;
		}
	}
	
	//set the stack buffer sizes
	setRecvBufferSize();
	setSendBufferSize();
	
	//save the addr we are sending data to
	sendaddr = info.address();
	
	//if a pointer to a parent thread was registered register the file
	//descriptor with the thread's run loop
	registerFD();
	
	//stop the socket from generating sigpipes
#ifndef __windows__
	int set = 1;
	::setsockopt(fdes, SOL_SOCKET, GS_NOSIGNAL, (void *)&set, sizeof(int));
#endif
	
	//emit the connected signal since the connection went through successfully
	if(info.socktype() == OO::Tcp) sigConnect();
	return true;
}

bool OSocket::listen(const OAddressInfo& info) {
	//make sure this socket class is not already maintaining a socket
	close();
	
	//if the container is empty return false
	if(info.isEmpty()) {
		return false;
	}
	
	_clearError();
	
	if((fdes = ::socket(info.family(), info.socktype(), info.protocol())) == -1) {
		sigError();
		return false;
	}
	
	int yes = 1;
#ifdef __windows__
	if(::setsockopt(fdes, SOL_SOCKET, SO_REUSEADDR, (const char*)&yes, sizeof(yes)) == -1) {
#else
	if(::setsockopt(fdes, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
#endif
		_close(fdes);
		sigError();
		return false;
	}
	
	OSockAddress addr = info.address();
	if(::bind(fdes, addr.constData(), addr.size()) == -1) {
		_close(fdes);
		sigError();
		return false;
	}
	
	//listen() only needs to be called if the socktype == Tcp
	if(info.socktype() == OO::Tcp) {
		int back;
		if ((back = ::listen(fdes, SOMAXCONN)) == -1) {
			_close(fdes);
			sigError();
			return false;
		}
		
#ifndef __windows__
		//reap all the dead processes
		struct sigaction sa;
		sa.sa_handler = sigchld_handler;
		sigemptyset(&sa.sa_mask);
		sa.sa_flags = SA_RESTART;
		if(sigaction(SIGCHLD, &sa, 0) == -1) {
			sigError();
		}
#endif
	}
	
	//register the file descriptor with the parent
	registerFD();
	
	//stop the socket from generating sigpipes
#ifndef __windows__
	int set = 1;
	setsockopt(fdes, SOL_SOCKET, GS_NOSIGNAL, (void *)&set, sizeof(int));
#endif
	
	return true;
}

bool OSocket::listen(const OSockAddress&	sock, 
			OO::SockType		type) {
	OAddressInfo info;
	info.family(sock.family());
	info.socktype(type);
	info.address(sock);
	
	return listen(info);
}

bool OSocket::listen(int port, 
					 OString ifn, 
					 OO::SockType type, 
					 OO::SockFamily family) {
	_clearError();
	
	int callerr = 0;
	OAddressList list = getAddressInfo(ifn, port, type, 
									   family, OO::Passive, &callerr);
	
	//if no items were in the list, an error was generated
	if(callerr || !list.size()) {
		sigError(callerr);
		return false;
	}
	
	//disable the error signal so erronious error signals are not
	//emmited while calling the other listen function
	disableErrorSig();
	for(unsigned int i=0; i<list.size(); i++) {
		OAddressInfo& info = list[i];
		if(!listen(info)) {
			continue;
		}
		break;
	}
	//enable the error signals so future error signals may be
	//emitted
	enableErrorSig();
	
	//if an error occured, report it
	if(error()) {
		sigError();
		return false;
	}
	
	return true;
}

OAddressList OSocket::getAddressInfo(OString addr, unsigned short port, 
									 OO::SockType type, OO::SockFamily family, 
									 OO::SockFlags flags,
									 int* errval) {
	OAddressInfo hints;
	hints.socktype(type);
	hints.family(family);
	hints.flags(flags);
	
	OString strport;
	strport<<port;
	
	return getAddressInfo(addr, strport, hints, errval);
}

OAddressList OSocket::getAddressInfo(OString addr, 
									 OString service, 
									 OAddressInfo hints, 
									 int* errval) {
	OAddressList list;
	
	addrinfo* servinfo = 0;
	addrinfo * p;
	
	unique_ptr<addrinfo> tmp;
	if(!hints.isEmpty()) {
		tmp.reset(new addrinfo);
		::memset(tmp.get(), 0, sizeof(addrinfo));
		tmp->ai_family = hints.family();
		tmp->ai_flags = hints.flags();
		tmp->ai_protocol = hints.protocol();
		tmp->ai_socktype = hints.socktype();
	}
	
	int ret = 0;
	int attempts = 0;
	do {
		if(addr.isEmpty()) {
			ret = getaddrinfo(0, service.toCString(), tmp.get(), &servinfo);
		} else {
			ret = getaddrinfo(addr.toCString(), service.toCString(), tmp.get(), &servinfo);
		}
		//no error was generated so break from the loop
		if(!ret) break;
		
		//attempts counter is used here so if EAI_AGAIN error is 
		//generated we can try a few more times before giving up, 
		//if we exceed 5 attempts then just give up
		attempts++;
		if(attempts > 5) {
#ifdef __windows__
			if(ret != 0) {
#else
			if(ret == EAI_SYSTEM) {
#endif
				if(errval) *errval = _error;
			} else { 
				if(errval) *errval = ret;
			}
			
			return list;
		}
	} while(ret == EAI_AGAIN);
	
	//if no host information was retrieved return an empty list
	if(!servinfo) {
#ifdef __windows__
		if(ret != 0) {
#else
		if(ret == EAI_SYSTEM) {
#endif
			if(errval) *errval = _error;
		} else { 
			if(errval) *errval = ret;
		}
		
		return list;
	}
	
	//convert the servinfo linked list to a OAddressList
	for(p = servinfo; p; p = p->ai_next) {
		list.push_back(p);
	}
	
	freeaddrinfo(servinfo);
	
	if(errval) 
		*errval = ret;
	return list;
}

OThread* OSocket::parent() {
	return par;
}

void OSocket::setParent(OThread *p) {
	this->par = p;
	registerFD();
}

bool OSocket::isEmpty() {
	
}

OByteArray OSocket::magicPacket(OMacAddress mac) {
	OByteArray pack;
	
	for(int i=0; i<6; i++)
		pack.write((unsigned char)0xff);
	
	for(int i=0; i<16; i++)
		pack.write(mac.constData(), mac.size());
	
	return pack;
}

int OSocket::backlog() {
	return SOMAXCONN;
}

bool OSocket::connected() {
	return conn;
}

OSockAddress OSocket::peerAddress() {
	return recvaddr;
}

void OSocket::setFileDescriptor(OO::HANDLE des) {
	//unregister the file descriptor with the parent thread
	//before we change it
	unregisterFD();
	
	fdes = (OO::SOCKET)des;
	
	//register the new file descriptor with the parent thread
	registerFD();
}

OString OSocket::hostName() {
	char tmp[200];
	gethostname(tmp, 200);
	return OString(tmp);
}

bool OSocket::hostName(OString name) {
#ifdef __windows__
	return false;
#else
	return ::sethostname(name.toCString(), name.length()) == 0;
#endif
}

ONetList OSocket::netList() {
	//get a list of the network interfaces
	ONetList interfaces;
	
#ifdef __windows__



#else
	//pointer to the head of the list of interfaces
	struct ifaddrs* ifaphead;
	//used to iterate through the list and get ipv4 and name
	struct ifaddrs* ifap;
	//used to iterate throught the list and get ipv6
	struct ifaddrs* ifap6;
	//used to iterate through the list and get mac address
	struct ifaddrs* ifapmac;
	
	
	
	//can't get interfaces so just return a blank list
	if(getifaddrs(&ifaphead) != 0) {
		return interfaces;
	}
	
	//get the inet4 address and name
	for(ifap = ifaphead; ifap; ifap = ifap->ifa_next) {
		if(ifap->ifa_addr->sa_family == AF_INET) {
			ONet tmpnet;
			//get the name
			tmpnet.ifa_name = OString(ifap->ifa_name);
			
			//get broadcast address
			tmpnet.ifa_addrbroad = ifap->ifa_broadaddr;
			
			//get the inet4 addr
			tmpnet.ifa_addr = ifap->ifa_addr;
			
			//get the subnet mask for the interface
			tmpnet.ifa_netmask = ifap->ifa_netmask;
			
			interfaces.push_back(tmpnet);
		}
	}
	
	//get the mac
	for(ifapmac = ifaphead; ifapmac; ifapmac = ifapmac->ifa_next) {
		if(ifapmac->ifa_addr->sa_family == AF_LINK) {
			for(unsigned i=0; i<interfaces.size(); i++) {
				if(interfaces[i].ifa_name == ifapmac->ifa_name) {
					//i don't know why 10 is the right offset to get 
					//the mac address but apparently it is. i could 
					//not find much documentation about this on the 
					//internet so i just printed out memory until i 
					//found something that looked like a mac and got 
					//the offset. if anything breaks on another 
					//linux system this may be it.
#ifdef __linux__
					memcpy(interfaces[i].ifa_mac.data(), 
						   ifapmac->ifa_addr->sa_data + 10, 
						   interfaces[i].ifa_mac.size());
#endif
					
#ifdef __apple__
					sockaddr_dl* sdl;
					sdl = (sockaddr_dl*)ifapmac->ifa_addr;
					memcpy(interfaces[i].ifa_mac.data(), 
						   LLADDR(sdl), interfaces[i].ifa_mac.size());
#endif
				}
			}
		}
	}
	
	//get the inet6 address
	for(ifap6 = ifaphead; ifap6; ifap6 = ifap6->ifa_next) {
		if(ifap6->ifa_addr->sa_family == AF_INET6) {
			for(unsigned i=0; i<interfaces.size(); i++) {
				if(interfaces[i].ifa_name == ifap6->ifa_name) {
					interfaces[i].ifa_addr6 = ifap6->ifa_addr;
				}
			}
		}
	}
	
	//free the system list of interfaces
	freeifaddrs(ifaphead);
#endif

	return interfaces;
}

int OSocket::error() const {
	return sockerr.code();
}

OString OSocket::strerror() const {
	return sockerr.string();
}


void* OSocket::get_in_addr(sockaddr *sa) {
	if(sa->sa_family == AF_INET) {//Inet4) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	} else if(sa->sa_family == AF_INET6) {//Inet6) {
		return &(((struct sockaddr_in6*)sa)->sin6_addr);
	}
	return 0;
}

void OSocket::connectFunc(function<void ()> cbk) {
	connectCbk = cbk;
}

void OSocket::disconnectFunc(function<void ()> cbk) {
	disconnectCbk = cbk;
}

void OSocket::errorFunc(function<void (OSockError)> cbk) {
	esigstat = true;
	errorCbk = cbk;
}

void OSocket::highPriorityFunc(function<void ()> cbk) {
	highPriorityCbk = cbk;
}

void OSocket::readyWriteFunc(function<void ()> cbk) {
	readyWriteCbk = cbk;
}

void OSocket::readyReadFunc(function<void ()> cbk) {
	cout<<"reg cbk" <<endl;
	readyReadCbk = cbk;
}

void OSocket::errorLoop() {
	int tmpe = 0;
	
	//get the error pending on the socket
	socklen_t len = sizeof(tmpe);
#ifdef __windows__
	::getsockopt(fdes, SOL_SOCKET, SO_ERROR, (char*)&tmpe, &len);
#else
	::getsockopt(fdes, SOL_SOCKET, SO_ERROR, &tmpe, &len);
#endif
	
	//if there is an error emit the signal
	if(tmpe) 
		sigError(tmpe);
	
}

void OSocket::readLoop() {
	int avail;
	if((avail = available())) {
		sigReadyRead();
	} else {
		unregisterFD();
		sigDisconnect();
	}
}

void OSocket::writeLoop() {
	//set the value of written to false so we can check it
	//after emitting the ready write signal, if written
	//is still false then no calls to write() were made
	//and the user is probably done writing bulk data
	written = false;
	
	sigReadyWrite();
	
	if(!written)
		disableReadyWrite();
	
}

void OSocket::incommingLoop() {
	int fd = accept(fdes, 0, 0);
	if(fd != -1 && fd != 0)
		sigIncomming(fd);
}

void OSocket::priorityLoop() {
	
}

#ifdef OO_QT

void OSocket::readyReadSlot(int socket) {
	if(readyReadPatch)
		readyReadPatch();
	else
		this->readLoop();
}

void OSocket::readyWriteSlot(int socket) {
	this->writeLoop();
}

#endif

void OSocket::incommingFunc(function<void (int)> cbk) {
	incommingCbk = cbk;
}

bool OSocket::waitForReadyRead(int msec) {
	timeval tmpt;
	tmpt.tv_sec = msec / 1000;
	tmpt.tv_usec = (msec % 1000) * 1000;
	
	fd_set tmpset;
	FD_ZERO(&tmpset);
	FD_SET(fdes, &tmpset);
	
	int ret = select(fdes + 1, &tmpset, 0, 0, &tmpt);
	
	if(ret > 0 && FD_ISSET(fdes, &tmpset)) {
		readLoop();
		return true;
	}
	
	return false;
}

bool OSocket::waitForReadyWrite(int msec) {
	timeval tmpt;
	tmpt.tv_sec = msec / 1000;
	tmpt.tv_usec = (msec % 1000) * 1000;
	
	fd_set tmpset;
	FD_ZERO(&tmpset);
	FD_SET(fdes, &tmpset);
	
	int ret = select(fdes + 1, 0, &tmpset, 0, &tmpt);
	
	if(ret > 0 && FD_ISSET(fdes, &tmpset)) {
		writeLoop();
		return true;
	}
	
	return false;
}

bool OSocket::waitForIncommingConnection(int msec) {
	timeval tmpt;
	tmpt.tv_sec = msec / 1000;
	tmpt.tv_usec = (msec % 1000) * 1000;
	
	fd_set tmpset;
	FD_ZERO(&tmpset);
	FD_SET(fdes, &tmpset);
	
	int ret = select(fdes + 1, &tmpset, 0, 0, &tmpt);
	
	if(ret > 0 && FD_ISSET(fdes, &tmpset)) {
		incommingLoop();
		return true;
	}
	
	return false;
}

void OSocket::setRecvBufferSize(int size) {
#ifdef __windows__
	::setsockopt(fdes, SOL_SOCKET, SO_RCVBUF, (const char*)&size, sizeof(size));
#else
	::setsockopt(fdes, SOL_SOCKET, SO_RCVBUF, &size, sizeof(size));
#endif
}

void OSocket::setSendBufferSize(int size) {
#ifdef __windows__
	::setsockopt(fdes, SOL_SOCKET, SO_SNDBUF, (const char*)&size, sizeof(size));
#else
	::setsockopt(fdes, SOL_SOCKET, SO_SNDBUF, &size, sizeof(size));
#endif
}

void OSocket::setBroadcast(bool b) {
	int yes = b;
#ifdef __windows__
	int length = sizeof(yes);
	::setsockopt(fdes, SOL_SOCKET, SO_BROADCAST, (const char*)&yes, length);
#else
	socklen_t length = sizeof(yes);
	::setsockopt(fdes, SOL_SOCKET, SO_BROADCAST, &yes, length);
#endif
}

bool OSocket::broadcast() {
	int yes = 0;
	socklen_t length = sizeof(yes);
#ifdef __windows__
	::getsockopt(fdes, SOL_SOCKET, SO_BROADCAST, (char*)&yes, &length);
#else
	::getsockopt(fdes, SOL_SOCKET, SO_BROADCAST, &yes, &length);
#endif
	return yes;
}

int OSocket::available() {
#ifdef __windows__
	u_long sum = 0;
#else
	int sum = 0;
#endif
	int back = _ioctl(fdes, FIONREAD, &sum);
	
	//an error was generated so handle it
	if(back < 0) {
		sigError();
		return 0;
	}
	
	return sum;
}

int OSocket::bytesAvailable() {
    return available();
}

OO::HANDLE OSocket::fileDescriptor() const {
	return (OO::HANDLE)fdes;
}

#ifndef __windows__
void OSocket::sigchld_handler(int s __attribute__((unused))) {
	while(waitpid(-1, 0, WNOHANG) > 0);
}
#endif

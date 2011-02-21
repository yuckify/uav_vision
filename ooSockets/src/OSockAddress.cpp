#include"OSockAddress.hpp"

OSockAddress::OSockAddress() {
	::memset(&addr, 0, sizeof(sockaddr_storage));
}

OSockAddress::OSockAddress(OString ip, 
						   unsigned short port, 
						   unsigned scope, 
						   unsigned flow) {
	::memset(&addr, 0, sizeof(sockaddr_storage));
	set(ip, port, scope, flow);
}

OSockAddress::OSockAddress(OO::SockFamily f) {
	::memset(&addr, 0, sizeof(sockaddr_storage));
	
	if(f == OO::Inet4) {
		::memset(&addr, 0, sizeof(sockaddr_storage));
		addr.ss_family = f;
	} else if(f == OO::Inet6) {
		::memset(&addr, 0, sizeof(sockaddr_storage));
		addr.ss_family = f;
	}
}

OSockAddress::~OSockAddress() {
}

sockaddr* OSockAddress::data() {
	return (sockaddr*)&addr;
}

const sockaddr* OSockAddress::constData() const {
	return (const sockaddr*)&addr;
}

int OSockAddress::size() const {
	if(addr.ss_family == OO::Inet4) {
		return sizeof(sockaddr_in);
	} else if(addr.ss_family == OO::Inet6) {
		return sizeof(sockaddr_in6);
	}
	return sizeof(sockaddr_storage);
}

void OSockAddress::clear() {
	::memset(&addr, 0, sizeof(sockaddr_storage));
}

void OSockAddress::set(OString ip, 
					   unsigned short port, 
					   unsigned scope, 
					   unsigned flow) {
	
	::memset(&addr, 0, sizeof(sockaddr_storage));
	
	if(ip.contains(":")) {//working with an ipv6 address
		//allocate the memory
		sockaddr_in6* tmp = (sockaddr_in6*)&addr;
		
		//initialize the sockaddr
		tmp->sin6_family = OO::Inet6;
		tmp->sin6_port = htons(port);
		tmp->sin6_flowinfo = flow;
		inet_pton(OO::Inet6, ip.toCString(), tmp->sin6_addr.s6_addr);
		tmp->sin6_scope_id = scope;
	} else {//else we are working with an ipv4 address
		//allocate the memory
		sockaddr_in* tmp = (sockaddr_in*)&addr;
		
		//initialize the sockaddr
		tmp->sin_family = OO::Inet4;
		tmp->sin_port = htons(port);
		inet_pton(OO::Inet4, ip.toCString(), &tmp->sin_addr.s_addr);
	}
}

void OSockAddress::operator=(const sockaddr* in) {
	this->set(*in);
}

void OSockAddress::set(const sockaddr& in) {
	//if the memory has already been allocated don't
	//allocate new memory if we don't need to
	if(in.sa_family == OO::Inet4) {
		set((sockaddr_in&)in);
	}
	else if(in.sa_family == OO::Inet6) {
		set((sockaddr_in6&)in);
	}
}

void OSockAddress::operator =(const sockaddr& in) {
	this->set(in);
}

void OSockAddress::set(const sockaddr_in& in) {
	::memcpy(&addr, &in, sizeof(sockaddr_in));
}

void OSockAddress::operator =(const sockaddr_in& in) {
	this->set(in);
}

void OSockAddress::set(const sockaddr_in6& in) {
	::memcpy(&addr, &in, sizeof(sockaddr_in6));
}

void OSockAddress::operator =(const sockaddr_in6& in) {
	this->set(in);
}

void OSockAddress::operator=(const sockaddr_in* in) {
	this->operator =(*in);
}

void OSockAddress::operator=(const sockaddr_in6* in) {
	this->operator =(*in);
}

void OSockAddress::operator=(const OSockAddress& in) {
	::memcpy(&addr, &in.addr, sizeof(sockaddr_storage));
}

bool OSockAddress::isEmpty() const {
	unsigned sum = 0;
	
	unsigned char* ptr = (unsigned char*)&addr;
	for(unsigned i=0; i<sizeof(sockaddr_storage); i++) {
		sum += ptr[i];
	}
	
	return !sum;
}

OO::SockFamily OSockAddress::family() const {
	return (OO::SockFamily)addr.ss_family;
}

void OSockAddress::family(OO::SockFamily f) {
	addr.ss_family = f;
}

bool OSockAddress::isIpv4() const {
	return addr.ss_family == OO::Inet4;
}

bool OSockAddress::isIpv6() const {
	return addr.ss_family == OO::Inet6;
}

int OSockAddress::port() const {
	if(addr.ss_family == OO::Inet4) {
		sockaddr_in* tmp = (sockaddr_in*)&addr;
		return ntohs(tmp->sin_port);
	} else if(addr.ss_family == OO::Inet6) {
		sockaddr_in6* tmp = (sockaddr_in6*)&addr;
		return ntohs(tmp->sin6_port);
	}
	return -1;
}

void OSockAddress::port(unsigned short p) {
	if(addr.ss_family == OO::Inet4) {
		sockaddr_in* tmp = (sockaddr_in*)&addr;
		tmp->sin_port = htons(p);
	} else if(addr.ss_family == OO::Inet6) {
		sockaddr_in6* tmp = (sockaddr_in6*)&addr;
		tmp->sin6_port = htons(p);
	}
}

OString OSockAddress::ipString() const {
	char xfer[50];
	if(addr.ss_family == OO::Inet4) {
		sockaddr_in* tmp = (sockaddr_in*)&addr;
		//get the address
		inet_ntop(tmp->sin_family, &tmp->sin_addr, xfer, 50);
		
	} else if(addr.ss_family == OO::Inet6) {
		sockaddr_in6* tmp = (sockaddr_in6*)&addr;
		//get the address
		inet_ntop(tmp->sin6_family, &tmp->sin6_addr, xfer, 50);
	}
	return OString(xfer);
}

void OSockAddress::ipString(OString a) {
	if(addr.ss_family == OO::Inet4) {
		sockaddr_in* tmp = (sockaddr_in*)&addr;
		inet_pton(OO::Inet4, a.toCString(), &tmp->sin_addr);
	} else if(addr.ss_family == OO::Inet6) {
		sockaddr_in6* tmp = (sockaddr_in6*)&addr;
		inet_pton(OO::Inet6, a.toCString(), &tmp->sin6_addr);
	}
}

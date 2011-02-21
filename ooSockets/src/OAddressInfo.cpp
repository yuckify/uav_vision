#include"OAddressInfo.hpp"

OAddressInfo::OAddressInfo() {
	ai_flags = (OO::SockFlags)0;
	ai_family = (OO::SockFamily)0;
	ai_socktype = (OO::SockType)0;
	ai_protocol = (OO::SockProto)0;
}

OAddressInfo::OAddressInfo(const addrinfo *other) {
	ai_flags = (OO::SockFlags)other->ai_flags;
	ai_family = (OO::SockFamily)other->ai_family;
	ai_socktype = (OO::SockType)other->ai_socktype;
	ai_protocol = (OO::SockProto)other->ai_protocol;
	ai_addr = other->ai_addr;
	if(other->ai_canonname) 
		ai_canonname = other->ai_canonname;
}

void OAddressInfo::operator=(const addrinfo* other) {
	ai_flags = (OO::SockFlags)other->ai_flags;
	ai_family = (OO::SockFamily)other->ai_family;
	ai_socktype = (OO::SockType)other->ai_socktype;
	ai_protocol = (OO::SockProto)other->ai_protocol;
	ai_addr = other->ai_addr;
	ai_canonname = other->ai_canonname;
}

OO::SockFlags OAddressInfo::flags() const {
	return ai_flags;
}

void OAddressInfo::flags(OO::SockFlags f) {
	ai_flags = f;
}

void OAddressInfo::appendFlags(OO::SockFlags f) {
	ai_flags = (OO::SockFlags)(ai_flags | f);
}

OO::SockFamily OAddressInfo::family() const {
	return ai_family;
}

void OAddressInfo::family(OO::SockFamily f) {
	ai_family = f;
}

OO::SockType OAddressInfo::socktype() const {
	return ai_socktype;
}

void OAddressInfo::socktype(OO::SockType t) {
	ai_socktype = t;
}

OO::SockProto OAddressInfo::protocol() const {
	return ai_protocol;
}

void OAddressInfo::protocol(OO::SockProto p) {
	ai_protocol = p;
}

OSockAddress OAddressInfo::address() const {
	return ai_addr;
}

void OAddressInfo::address(const OSockAddress& addr) {
	ai_addr = addr;
}

OString OAddressInfo::name() const {
	return ai_canonname;
}

void OAddressInfo::name(OString str) {
	ai_canonname = str;
}

bool OAddressInfo::isEmpty() const {
	return !ai_flags && !ai_family && !ai_socktype 
			&& !ai_protocol && ai_addr.isEmpty()
			&& ai_canonname.isEmpty();
}

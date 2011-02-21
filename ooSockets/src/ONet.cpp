#include"ONet.hpp"

ONet::ONet() {
	
}

OString ONet::name() const {
	return ifa_name;
}

void ONet::name(OString str) {
	ifa_name = str;
}

OString ONet::macString() const {
	return ifa_mac.str();
}

OMacAddress ONet::mac() const {
	return ifa_mac;
}

const char* ONet::constMacData() const {
	return ifa_mac.constData();
}

void ONet::mac(const OMacAddress &data) {
	ifa_mac = data;
}

void ONet::ipAddress(const OSockAddress &ip) {
	if(ip.isIpv4()) {
		ifa_addr = ip;
	} else if(ip.isIpv6()) {
		ifa_addr6 = ip;
	}
}

OSockAddress ONet::ipAddress() const {
	if(!ifa_addr.isEmpty()) {
		return ifa_addr;
	} else if(!ifa_addr6.isEmpty()) {
		return ifa_addr;
	}
	return OSockAddress();
}

OString ONet::ipString() const {
	if(!ifa_addr.isEmpty())
		return ifa_addr.ipString();
	else if(!ifa_addr6.isEmpty())
		return ifa_addr6.ipString();
	
	return "";
}

OString ONet::ip4String() const {
	return ifa_addr.ipString();
}

void ONet::ip4Address(const OSockAddress& ip) {
	ifa_addr = ip;
}

const OSockAddress& ONet::ip4Address() const {
	return ifa_addr;
}

OSockAddress& ONet::ip4Address() {
	return ifa_addr;
}

OString ONet::ip6String()  const{
	return ifa_addr6.ipString();
}

void ONet::ip6Address(const OSockAddress& ip) {
	ifa_addr6 = ip;
}

const OSockAddress& ONet::ip6Address() const {
	return ifa_addr6;
}

OSockAddress& ONet::ip6Address() {
	return ifa_addr6;
}

OString ONet::ipBroadcast() const {
	return ifa_addrbroad.ipString();
}

void ONet::ipBroadcast(const OSockAddress& ip) {
	ifa_addrbroad = ip;
}

void ONet::subnetMask(const OSockAddress& mask) {
	ifa_netmask = mask;
}

OSockAddress ONet::subnetMask() {
	return ifa_netmask;
}

bool ONet::isEmpty() const {
	return ifa_name.isEmpty() && ifa_mac.isEmpty() && 
			ifa_addr.isEmpty() && ifa_addr6.isEmpty() && 
			ifa_addrbroad.isEmpty();
}

unsigned ONet::index() const {
	return if_nametoindex(name().toCString());
}

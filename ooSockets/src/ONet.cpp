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

ONetList ONet::netList() {
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

#include"OUdpSocket.hpp"

OUdpSocket::OUdpSocket(OThread* parent) : OSocket(parent) {
	
}

bool OUdpSocket::bind(int port, OString addr, OO::SockFamily family) {
	if(!OSocket::listen(port, "", OO::Udp, family))
		return false;
	
	if(error())
		return false;
	
	int errval = 0;
	OAddressList list = 
			OSocket::getAddressInfo(addr, port, OO::Udp, OO::NullFamily, 
												OO::Passive, &errval);
	
	if(!errval && list.size() > 0)
		sendaddr = list.filterByType(OO::Udp)[0].address();
	else if(errval)
		sigError(errval);
	
	return true;
}

bool OUdpSocket::bind(OSockAddress addr) {
	if(!OSocket::listen(addr, OO::Udp))
		return false;
	
	if(error())
		return false;
	
	sendaddr = addr;
	
	return true;
}

bool OUdpSocket::bind(OAddressInfo info) {
	if(!OSocket::listen(info))
		return false;
	
	if(error())
		return false;
	
	sendaddr = info.address();
	return true;
}

void OUdpSocket::sendTo(unsigned short port, 
						OString addr, 
						OO::SockFamily family) {
	OSocket::connect(addr, port, OO::Udp, family);
}

void OUdpSocket::sendTo(OSockAddress addr) {
	OSocket::connect(addr, OO::Udp);
}

void OUdpSocket::sendTo(OAddressInfo addr) {
	addr.socktype(OO::Udp);
	OSocket::connect(addr);
}

void OUdpSocket::sendMulticast(unsigned short port, OString group) {
	OSockAddress addr(group, port);
	sendMulticast(addr);
}

void OUdpSocket::sendMulticast(const OSockAddress &addr) {
	OSocket::connect(addr, OO::Udp);
}

bool OUdpSocket::listenMulticast(unsigned short port, 
								 OString group,
								 OString ifa) {	
	OSockAddress gaddr(group, port);
	
	if(!ifa.isEmpty()) {
		OSockAddress ifaddr(ifa, port);
		return listenMulticast(gaddr, ifaddr);
	}
	
	return listenMulticast(gaddr, OSockAddress());
}

bool OUdpSocket::listenMulticast(unsigned short port, 
								 OString group, 
								 ONet ifa) {
	OSockAddress gaddr(group, 0);
	
	ifa.ip4Address().port(port);
	ifa.ip6Address().port(port);
	
	return listenMulticast(gaddr, ifa);
}

bool OUdpSocket::listenMulticast(OSockAddress group, ONet ifa) {
	if(group.family() == OO::Inet4) {
		return listenMulticast(group, ifa.ip4Address());
	}
	
	return listenMulticast(group, ifa.ip6Address());
}

bool OUdpSocket::listenMulticast(OSockAddress group, OSockAddress ifa) {
	//disable the error signal so erronious error signals are not
	//emmited while calling the other listen function
	disableErrorSig();
	
	bool ret = OSocket::listen(group.port(), "", OO::Udp, OO::NullFamily);
	
	//enable the error signals so future error signals may be
	//emitted
	enableErrorSig();
	
	if(!ret || error()) {
		sigError();
		return false;
	}
	
	bool rec = addMembership(group, ifa);
	
	return rec;
}

bool OUdpSocket::addMembership(OSockAddress group, OSockAddress ifa) {
	if(ifa.isEmpty()) {
		//the variable ifa is empty so this means we are going to listen
		//on all interfaces
		if(group.isIpv4()) {
#ifdef __apple__
			//the darwin kernel is multihomed so we have to add each
			//multicast capable interface to our listening socket
			//get the net list
			ONetList nets = OSocket::netList();
			
			//add the groups
			for(auto i=nets.begin(); i<nets.end(); i++) {
				addMembership(group, i->ip4Address());
			}
#else
			//create the variable so we may join a multicast group, then clear it
			ip_mreq mreq;
			::bzero(&mreq, sizeof(ip_mreq));
			
			//set the group address
			mreq.imr_multiaddr.s_addr = ((sockaddr_in&)group.addr).sin_addr.s_addr;
			
			//set the interface address, all interfacess
			mreq.imr_interface.s_addr = INADDR_ANY;
			
			//use setsockopt() to request that the kernel join a multicast group
#ifdef __windows__
			if(setsockopt(fdes, IPPROTO_IP, IP_ADD_MEMBERSHIP, (const char*)&mreq, sizeof(mreq)) < 0) {
#else
			if(setsockopt(fdes, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
#endif
				close();
				sigError();
				return false;
			}
#endif
		} else if(group.isIpv6()) {
#ifdef __apple__
			//the darwin kernel is multihomed so we have to add each
			//multicast capable interface to our listening socket
			//get the net list
			ONetList nets = OSocket::netList();
			
			//add the groups
			for(auto i=nets.begin(); i<nets.end(); i++) {
				addMembership(group, i->ip6Address());
			}
#else
			//create the variable so we may join a multicast group, then clear it
			ipv6_mreq mreq;
			::bzero(&mreq, sizeof(ipv6_mreq));
			
			//set the group address
#ifdef __linux__
			mreq.ipv6mr_multiaddr.__in6_u = ((sockaddr_in6&)group.addr).sin6_addr.__in6_u;
#elif defined(__windows__)
			mreq.ipv6mr_multiaddr.u = ((sockaddr_in6&)group.addr).sin6_addr.u;
#else
			mreq.ipv6mr_multiaddr.__u6_addr = ((sockaddr_in6&)group.addr).sin6_addr.__u6_addr;
#endif
			
			//set the interface index, all interfaces
			mreq.ipv6mr_interface = INADDR_ANY;
			
			//use setsockopt() to request that the kernel join a multicast group
#ifdef __windows__
			if(setsockopt(fdes, IPPROTO_IP, IPV6_JOIN_GROUP, (const char*)&mreq, sizeof(mreq)) < 0) {
#else
			if(setsockopt(fdes, IPPROTO_IP, IPV6_JOIN_GROUP, &mreq, sizeof(mreq)) < 0) {
#endif
				close();
				sigError();
				return false;
			}
#endif
		}
	} else {
		//the variable ifa is not empty so this means listen on the
		//specified interface
		if(group.isIpv4()) {
			//create the variable so we may join a multicast group, then clear it
			ip_mreq mreq;
			::bzero(&mreq, sizeof(ip_mreq));
			
			//set the group address
			mreq.imr_multiaddr.s_addr = ((sockaddr_in&)group.addr).sin_addr.s_addr;
			
			//make sure the interface address is an ipv4 address
			//if not get the proper address for the requested interface
			OSockAddress ifaddr;
			if(ifa.isIpv6()) {
				ONetList nets = OSocket::netList().filterByIp4(ifa.ipString());
				if(nets.size() > 0) ifaddr = nets[0].ip4Address();
				else return false;
			} else {
				ifaddr = ifa;
			}
			
			//set the interface address
			mreq.imr_interface.s_addr = ((sockaddr_in&)ifaddr.addr).sin_addr.s_addr;
			
			//use setsockopt() to request that the kernel join a multicast group
#ifdef __windows__
			if(setsockopt(fdes, IPPROTO_IP, IP_ADD_MEMBERSHIP, (const char*)&mreq, sizeof(mreq)) < 0) {
#else
			if(setsockopt(fdes, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
#endif
				sigError();
				return false;
			}
		} else if(group.isIpv6()) {
			//create the variable so we may join a multicast group, then clear it
			ipv6_mreq mreq;
			::bzero(&mreq, sizeof(ipv6_mreq));
			
			//set the group address
#ifdef __linux__
			mreq.ipv6mr_multiaddr.__in6_u = ((sockaddr_in6&)group.addr).sin6_addr.__in6_u;
#elif defined(__windows__)
			mreq.ipv6mr_multiaddr.u = ((sockaddr_in6&)group.addr).sin6_addr.u;
#else
			mreq.ipv6mr_multiaddr.__u6_addr = ((sockaddr_in6&)group.addr).sin6_addr.__u6_addr;
#endif
			
			//we need to get the interface
			ONetList nets = OSocket::netList().filterByIp4(ifa.ipString());
			ONet net;
			if(nets.size() > 0) net = nets[0];
			else return false;
			
			//set the interface index
			mreq.ipv6mr_interface = net.index();
			
			//use setsockopt() to request that the kernel join a multicast group
#ifdef __windows__
			if(setsockopt(fdes, IPPROTO_IP, IPV6_JOIN_GROUP, (const char*)&mreq, sizeof(mreq)) < 0) {
#else
			if(setsockopt(fdes, IPPROTO_IP, IPV6_JOIN_GROUP, &mreq, sizeof(mreq)) < 0) {
#endif
				sigError();
				return false;
			}
		}
	}
	
	sockerr = 0;
	return true;
}

bool OUdpSocket::addMembership(OString group, OString ifa) {
	return false;
}

void OUdpSocket::setTTL(unsigned int len) {
	socklen_t socklen = sizeof(len);
#ifdef __windows__
	setsockopt(fdes, IPPROTO_IP, IP_MULTICAST_TTL, (const char*)&len, socklen);
#else
	setsockopt(fdes, IPPROTO_IP, IP_MULTICAST_TTL, &len, socklen);
#endif
}

unsigned int OUdpSocket::getTTL() {
	unsigned int len = 0;
	socklen_t socklen = sizeof(len);
#ifdef __windows__
	getsockopt(fdes, IPPROTO_IP, IP_MULTICAST_TTL, (char*)&len, &socklen);
#else
	getsockopt(fdes, IPPROTO_IP, IP_MULTICAST_TTL, &len, &socklen);
#endif
	return len;
}

bool OUdpSocket::broadcast(bool s) {
	int value = s;
	socklen_t socklen = sizeof(value);
#ifdef __windows__
	return !::setsockopt(fdes, SOL_SOCKET, SO_BROADCAST, (const char*)&value, socklen);
#else
	return !::setsockopt(fdes, SOL_SOCKET, SO_BROADCAST, &value, socklen);
#endif
}

bool OUdpSocket::broadcast() {
	int value = 0;
	socklen_t socklen = sizeof(value);
#ifdef __windows__
	return ::getsockopt(fdes, SOL_SOCKET, SO_BROADCAST, (char*)&value, &socklen);
#else
	return ::getsockopt(fdes, SOL_SOCKET, SO_BROADCAST, &value, &socklen);
#endif
}

void OUdpSocket::txMagicPacket(OSockAddress& broad, OMacAddress& mac) {
	OByteArray pack = OSocket::magicPacket(mac);
	
	OUdpSocket sock;
	sock.sendTo(broad);
	if(!sock.broadcast(true)) return;
	sock.write(pack);
}

OByteArray OUdpSocket::read(int len) {
	return read(recvaddr, len);
}

OByteArray OUdpSocket::read(OSockAddress& addr, int len) {
	if(len <= 0) {
		return OByteArray();
	}
	
	OByteArray ba;
	
	//make sure the container can fit all the data
	//we are about to read
	ba.resize(len);
	addr.clear();
	socklen_t socklen = addr.size();
	int recvlen = recvfrom(fdes, ba.tellData(), len, 0, 
						   addr.data(), &socklen);
	
	if(recvlen > 0) {
		ba.advanceSize(recvlen);
	}
	return ba;
}

OByteArray OUdpSocket::readAll() {
	return read(available());
}

OByteArray OUdpSocket::readAll(OSockAddress& addr) {
	return read(addr, available());
}

int OUdpSocket::write(const OByteArray &data) {
	int ret = -1;
	
	if((ret = ::sendto(fdes, data.constData(), data.size(), 0, 
			  sendaddr.constData(), sendaddr.size())) < 0) {
		sigError();
	}
	
	if(ret > 0) 
		written = true;
	
	return ret;
}

int OUdpSocket::write(const OSockAddress &addr, const OByteArray &data) {
	int ret = -1;
	
	if((ret = ::sendto(fdes, data.constData(), data.size(), 0, 
			  addr.constData(), addr.size())) < 0) {
		sigError();
	}
	
	if(ret > 0) 
		written = true;
	
	return ret;
}

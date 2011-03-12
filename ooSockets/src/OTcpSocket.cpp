#include"OTcpSocket.hpp"

OTcpSocket::OTcpSocket(OThread *parent) : OSocket(parent) {
	
}

bool OTcpSocket::connect(OString addr, 
						 unsigned short port, 
						 OO::SockFamily family) {
	return OSocket::connect(addr, port, OO::Tcp, family);
}

bool OTcpSocket::connect(const OSockAddress& addr) {
	return OSocket::connect(addr, OO::Tcp);
}

bool OTcpSocket::connect(const OAddressInfo& info) {
	OAddressInfo addr = info;
	addr.socktype(OO::Tcp);
	
	return OSocket::connect(addr);
}

OByteArray OTcpSocket::read(int len) {
	OByteArray ba;
	
	if(len <= 0) {
		return ba;
	}
	
	//make sure the container can fit all the data
	//we are about to read
	ba.resize(len);
	int recvlen = recv(fdes, ba.tellData(), len, 0);
	if(recvlen != -1) {
		ba.advanceSize(len);
	} else {
		//an error occured so report it
		sigError();
	}
	
	return ba;
}

OByteArray OTcpSocket::readAll() {
	return read(available());
}

int OTcpSocket::write(OByteArray &data) {
	int ret = -1;
	
	if((ret = send(fdes, data.constData(), data.size(), 0)) == -1) {
		unregisterFD();
		sigDisconnect();
		sigError();
		return -1;
	} else {
		written = true;
	}
	
	return ret;
}

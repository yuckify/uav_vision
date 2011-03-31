#include"OTcpSocket.hpp"

OTcpSocket::OTcpSocket(OThread *parent) : OSocket(parent) {
	
}

OTcpSocket::OTcpSocket(QObject *parent) : OSocket(parent) {
	
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

int OTcpSocket::read(OByteArray& data, int len) {
	if(len <= 0) return 0;
	
	data.resize(len);
	int recvlen = recv(fdes, data.tellData(), len, 0);
	if(recvlen != -1) {
		data.advanceSize(recvlen);
		data.seek(recvlen, OO::cur);
	} else {
		//an error occured so report it
		sigError();
	}
	
	return recvlen;
}

int OTcpSocket::readAll(OByteArray &data) {
	return read(data, available());
}

OByteArray OTcpSocket::read(int len) {
	OByteArray ba;
	
	if(len <= 0) {
		return ba;
	}
	
	//make sure the container can fit all the data
	//we are about to read
	ba.resize(len);
	int recvlen = recv(fdes, ba.data(), len, 0);
	if(recvlen != -1) {
		ba.advanceSize(recvlen);
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
	return write(data, data.dataLeft());
}

int OTcpSocket::write(OByteArray &data, int len) {
	int ret = -1;
	if(!(data.size() && (len > 0))) return 0;
	
	ret = send(fdes, data.constTellData(), len, 0);
	if(ret == -1) {
		unregisterFD();
		sigDisconnect();
		sigError();
		return -1;
	} else {
		written = true;
		data.seek(ret, OO::cur);
	}
	
	return ret;
}

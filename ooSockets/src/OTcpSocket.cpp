#include"OTcpSocket.hpp"

OTcpSocket::OTcpSocket(OThread *parent) : OSocket(parent) {
	
}

#ifdef OO_QT
OTcpSocket::OTcpSocket(QObject *parent) : OSocket(parent) {
	
}
#endif

OTcpSocket::OTcpSocket(const OTcpSocket &other) : OSocket(other) {
	
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
	
	data.resize(data.size() + len);
	int recvlen = recv(fdes, data.tellData(), len, 0);
	if(recvlen >= 0) {
		data.resize(data.size() - len + recvlen);
		data.seek(recvlen, OO::cur);
	} else {
		data.resize(data.size() - len);
		//an error occured so report it
		sigDisconnect();
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
	ba.resize(ba.size() + len);
	int recvlen = recv(fdes, ba.data(), len, 0);
	if(recvlen >= 0) {
		ba.resize(ba.size() - len + recvlen);
	} else {
		ba.resize(ba.size() - len);
		//an error occured so report it
		sigDisconnect();
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

bool OTcpSocket::connected() {
	int error = 0;
	socklen_t len = sizeof(error);
	int retval = getsockopt(fdes, SOL_SOCKET, SO_ERROR, &error, &len);
	return !retval;
}

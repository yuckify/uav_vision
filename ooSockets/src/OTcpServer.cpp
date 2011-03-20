#include"OTcpServer.hpp"

OTcpServer::OTcpServer(OThread *parent) : OSocket(parent) {
	
}

bool OTcpServer::listen(const OAddressInfo& info) {
	return OSocket::listen(info);
}

bool OTcpServer::listen(const OSockAddress&	sock) {
	return OSocket::listen(sock, OO::Tcp);
}

bool OTcpServer::listen(int	port, 
			OString			ifn, 
			OO::SockFamily	family) {
	return OSocket::listen(port, ifn, OO::Tcp, family);
}

OByteArray OTcpServer::read( int len) {
	return OByteArray();
}

OByteArray OTcpServer::readAll() {
	return OByteArray();
}
int OTcpServer::write(OByteArray &data) {
	return 0;
}

void OTcpServer::readLoop() {
	incommingLoop();
}

void OTcpServer::writeLoop() {
	incommingLoop();
}

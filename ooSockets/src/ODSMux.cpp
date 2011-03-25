#include "ODSMux.h"

/*
ODataStream ODSMux::connect(OString addr, unsigned short port) {
	ODataStream tmp(q_parent);
	tmp.connect(addr, port);
	q_ques.push_back(tmp);
	return tmp;
}

ODataStream ODSMux::connect(const OSockAddress& addr) {
	ODataStream tmp(q_parent);
	tmp.connect(addr);
	q_ques.push_back(tmp);
	return tmp;
}

ODataStream ODSMux::connect(const OAddressInfo& info) {
	ODataStream tmp(q_parent);
	tmp.connect(info);
	q_ques.push_back(tmp);
	return tmp;
}

ODSMux::ODSMux(OThread *parent) {
	q_parent = parent;
	
}

ODSMux::ODSMux(OThread *parent, OTcpSocket *socket) {
	q_parent = parent;
}

ODSMux::ODSMux(OThread *parent, int fd) {
	q_parent = parent;
}

bool ODSMux::listen(const OAddressInfo& info) {
	return q_serv.listen(info);
}

bool ODSMux::listen(const OSockAddress&	sock) {
	return q_serv.listen(sock);
}

bool ODSMux::listen(int port, OString ifn) {
	return q_serv.listen(port, ifn);
}

int ODSMux::size() const {
	return q_ques.size();
}

ODataStream ODSMux::operator [](int i) {
	return q_ques[i];
}
*/

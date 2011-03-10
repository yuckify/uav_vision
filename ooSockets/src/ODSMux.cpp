#include "ODSMux.h"

ODSMux::ODSMux(OThread *parent) {
	q_parent = parent;
	
	//setup the socket
	q_sock.reset(new OTcpSocket(parent));
	q_sock->readyReadFunc(bind(&ODSMux::readyRead, this));
	q_sock->readyWriteFunc(bind(&ODSMux::readyWrite, this));
}

ODSMux::ODSMux(OThread *parent, OTcpSocket *socket) {
	q_parent = parent;
	
	//setup the socket
	q_sock.reset(socket);
	q_sock->setParent(parent);
	q_sock->readyReadFunc(bind(&ODSMux::readyRead, this));
	q_sock->readyWriteFunc(bind(&ODSMux::readyWrite, this));
}

ODSMux::ODSMux(OThread *parent, int fd) {
	q_parent = parent;
	
	//setup the socket
	q_sock.reset(new OTcpSocket(parent));
	q_sock->fileDescriptor(fd);
	q_sock->readyReadFunc(bind(&ODSMux::readyRead, this));
	q_sock->readyWriteFunc(bind(&ODSMux::readyWrite, this));
}

uint16_t ODSMux::addQueue() {
	for(int i=0; i<ques.size(); i++) {
		if(!ques[i].get()) {
			addQueue(i);
			return i;
		}
	}
	
	int handle = ques.size();
	addQueue(handle);
	return handle;
}

void ODSMux::addQueue(uint16_t handle, int priority) {
	if(!queueExists(handle)) {
		shared_ptr<mux> tmp;
		while(ques.size() <= handle) ques.push_back(tmp);
		
		tmp.reset(new mux);
		tmp->priority = priority;
		ques[handle] = tmp;
		pque.push_back(handle);
		
		this->regenPriorities();
	}
}

void ODSMux::removeQueue(uint16_t handle) {
	if(this->queueExists(handle)) {
		for(auto i=pque.begin(); i<pque.end(); i++) {
			if(*i == handle) {
				pque.erase(i);
				break;
			}
		}
		
		ques[handle].reset();
	}
}

bool ODSMux::queueExists(uint16_t handle) {
	if(handle >= ques.size()) return false;
	if(!ques[handle].get()) return false;
	return true;
}

void ODSMux::setQueuePriority(uint16_t handle, int priority) {
	if(this->queueExists(handle)) {
		ques[handle]->priority = priority;
		
		this->regenPriorities();
	}
}

void ODSMux::setPacketSize(uint16_t size) {
	q_packetsize = size;
}

void ODSMux::regenPriorities() {
	sort(pque.begin(), pque.end(), [&ques](uint16_t a, uint16_t b) -> bool {
		return ques[b]->priority > ques[a]->priority;
	});
}

void ODSMux::readyRead() {
	
}

void ODSMux::readyWrite() {
	
}

void ODSMux::incomming(int fd) {
	q_sock->close();
	
	q_sock->fileDescriptor(fd);
}

void ODSMux::setPacketHandler(uint16_t packetId, function<void (OByteArray)> cbk) {
	while(q_handlers.size() <= packetId) q_handlers.push_back(NULL);
	q_handlers[packetId] = cbk;
}

bool ODSMux::connect(OString addr, unsigned short port) {
	return q_sock->connect(addr, port);
}

bool ODSMux::connect(const OSockAddress& addr) {
	return q_sock->connect(addr);
}

bool ODSMux::connect(const OAddressInfo& info) {
	return q_sock->connect(info);
}

bool ODSMux::listen(const OAddressInfo& info) {
	return q_serv->listen(info);
}

bool ODSMux::listen(const OSockAddress&	sock) {
	return q_serv->listen(sock);
}

bool ODSMux::listen(int port, OString ifn) {
	return q_serv->listen(port, ifn);
}

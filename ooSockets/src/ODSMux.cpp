#include "ODSMux.h"

ODataStream::ODataStream(OThread* parent) {
	//create the memory
	q_mem.reset(new ODataStreamMem(parent));
	
	//initialize the socket callbacks
	q_mem->q_sock.readyReadFunc(bind(&ODataStream::readyRead, this));
	q_mem->q_sock.readyWriteFunc(bind(&ODataStream::readyWrite, this));
	
	
	
}

void ODataStream::setRecvHandler(uint16_t packetId, function<void (OByteArray)> cbk) {
	if(isInit()) {
		while(q_mem->q_handlers.size() <= packetId) 
			q_mem->q_handlers.push_back(NULL);
		
		q_mem->q_handlers[packetId] = cbk;
	}
}

uint16_t ODataStream::priority(uint16_t packetId) {
	if(isInit()) {
		if(q_mem->q_ique.size() > packetId)
			return q_mem->q_ique[packetId].priority();
	}
	return 0;
}

bool ODataStream::sendHandlerIsSet(uint16_t packetId) {
	if(isInit()) {
		if(q_mem->q_ique.size() > packetId)
			return q_mem->q_ique[packetId].isInit();
	}
	return false;
}

bool ODataStream::recvHandlerIsSet(uint16_t packetId) {
	if(isInit()) {
		return q_mem->q_handlers[packetId] != NULL;
	}
	return false;
}

void ODataStream::write(uint16_t packetId, OByteArray data) {
	if(isInit()) {
		if(q_mem->q_ique.size() > packetId)
			q_mem->q_ique[packetId].write(data);
	}
}

void ODataStream::setSendHandler(uint16_t packetId, uint16_t priority) {
	if(isInit()) {
		while(q_mem->q_ique.size() <= packetId) 
			q_mem->q_ique.push_back(ODSQueue());
		
		while(q_mem->q_pque.size() <= priority)
			q_mem->q_pque.push_back(ODSQueue());
		
		if(!q_mem->q_pque[priority].isInit()) {
			//a queue with this priority does not exist
			q_mem->q_pque[priority] = ODSQueue(q_mem.get());
			q_mem->q_pque[priority].setPriority(priority);
		}
		
		//set that we want to use this specific queue with this packetId
		q_mem->q_ique[packetId] = q_mem->q_pque[priority];
	}
}

bool ODataStream::isInit() {
	return q_mem.get();
}

bool ODataStream::isEmpty() {
	if(isInit()) {
		return !q_mem->q_pque.size() && !q_mem->q_ique.size() &&
				!q_mem->q_handlers.size() && q_mem->q_sock.isEmpty();
	}
	return false;
}

OThread* ODataStream::parent() const {
	return q_mem->q_parent;
}

void ODataStream::setParent(OThread* parent) {
	q_mem->q_parent = parent;
}

bool ODataStream::connect(OString addr, unsigned short port) {
	return q_mem->q_sock.connect(addr, port);
}

bool ODataStream::connect(const OSockAddress& addr) {
	return q_mem->q_sock.connect(addr);
}

bool ODataStream::connect(const OAddressInfo& info) {
	return q_mem->q_sock.connect(info);
}

void ODataStream::readyRead() {
	do {
		//first check if we need to read in the header for the next packet
		if(q_mem->q_readhead) {
			//read in the header
			q_mem->q_head.append(q_mem->q_sock.read(sizeof(ODSPacketLength) + 
											   sizeof(ODSPacketType) - 
											   q_mem->q_head.size()));
			
			//readhead = true if head.size() != sizeof(header information)
			//this bool specifies if we have read in the whole header or
			//not, true (have not read in header) false (have read in header)
			q_mem->q_readhead = q_mem->q_head.size() != 
								  (sizeof(ODSPacketLength) + sizeof(ODSPacketType));
			
			//we have not finished reading in the head, so return and wait
			//for more data
			if(q_mem->q_readhead) return;
			
			//deserialize the header information
			q_mem->q_head>>q_mem->q_length >>q_mem->q_type;
			q_mem->q_head.clear();
		}
		
		//read in some data
		q_mem->q_data.append(q_mem->q_sock.read(q_mem->q_length - 
												q_mem->q_data.size()));
		
		if(q_mem->q_data.size() == q_mem->q_length) {
			//if a function is set for this packet signature, call it
			if(q_mem->q_handlers[q_mem->q_type])
				q_mem->q_handlers[q_mem->q_type](q_mem->q_data);
			
			//we are done with this data
			q_mem->q_data.clear();
			q_mem->q_readhead = true;
		}
		
	} while(q_mem->q_data.size() == q_mem->q_length);
}

void ODataStream::readyWrite() {
	
}

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

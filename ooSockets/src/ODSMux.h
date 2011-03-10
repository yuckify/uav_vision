#ifndef ODSMUX_H
#define ODSMUX_H

#include<deque>
#include<memory>
#include<vector>
#include<algorithm>

using namespace std;

#include<OByteArray.hpp>
#include<OTcpSocket.hpp>
#include<OThread.hpp>
#include<OTcpServer.hpp>

class ODSMux {
public:
	ODSMux(OThread* parent);
	ODSMux(OThread* parent, OTcpSocket* socket);
	ODSMux(OThread* parent, int fd);
	
	
	uint16_t addQueue();
	void addQueue(uint16_t handle, int priority = 0);
	void removeQueue(uint16_t handle);
	bool queueExists(uint16_t handle);
	
	void setQueuePriority(uint16_t handle, int priority);
	void setPacketSize(uint16_t size);
	
	void setPacketHandler(uint16_t packetId, function<void (OByteArray)> cbk);
	
	bool connect(OString addr, unsigned short port);
	
	bool connect(const OSockAddress& addr);
	
	bool connect(const OAddressInfo& info);
	
	bool listen(int port, OString ifn = "");
	
	bool listen(const OAddressInfo& info);
	
	bool listen(const OSockAddress&	sock);
	
protected:
	void regenPriorities();
	
	void readyRead();
	void readyWrite();
	void incomming(int fd);
	
	struct mux {
		deque<OByteArray> que;
		int priority;
	};
	
	vector<shared_ptr<mux>>				ques;
	vector<uint16_t>					pque;
	int									q_packetsize;
	unique_ptr<OTcpSocket>				q_sock;
	unique_ptr<OTcpServer>				q_serv;
	OThread*							q_parent;
	vector<function<void (OByteArray)>>	q_handlers;
	
};

#endif // ODSMUX_H

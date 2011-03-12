#ifndef ODSMUX_H
#define ODSMUX_H

#include<deque>
#include<memory>
#include<list>
#include<vector>
#include<algorithm>
#include<mutex>

using namespace std;

#include<OByteArray.hpp>
#include<OTcpSocket.hpp>
#include<OThread.hpp>
#include<OTcpServer.hpp>
#include<OString.hpp>

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
	
	class ODSQueueList;
	class ODSQueue;
	
	class ODSQueueBase {
		friend class ODSQueue;
	protected:
		virtual void regenPriorities();
	};
	
	class ODSQueue {
		friend class ODSQueueList;
	protected:
		class ODSQueueMem {
		public:
			ODSQueueMem(ODSQueueBase* parent) : q_parent(parent) {}
			
			list<OByteArray>					q_que;
			mutex								q_mutex;
			ODSQueueBase*						q_parent;
			int									q_priority;
			OString								q_name;
		};
	public:
		ODSQueue() {}
		
		void setPriority(int p) {
			if(q_mem.get()) q_mem->q_priority = p;
			q_mem->q_parent->regenPriorities();
		}
		
		int priority() {
			if(q_mem.get()) return q_mem->q_priority;
		}
		
		bool isEmpty() {
			return !q_mem.get();
		}
		
	protected:
		ODSQueue(ODSQueueBase* parent) {
			q_mem.reset(new ODSQueueMem(parent));
		}
		
		shared_ptr<ODSQueueMem>				q_mem;
	};
	
	class ODSQueueList {
	protected:
		class ODSQueueListMem : public ODSQueueBase {
			friend class ODSQueueList;
		public:
			ODSQueueListMem(OThread* parent) : q_sock(parent) {
				q_parent = parent;
			}
		
		protected:
			//current packet we are reading in
			uint32_t							q_type;
			uint32_t							q_length;
			
			vector<ODSQueue>					q_que;
			vector<uint16_t>					q_pque;
			int									q_packetsize;
			OTcpSocket							q_sock;
			OThread*							q_parent;
			vector<function<void (OByteArray)>>	q_handlers;
			OString								q_name;
		};
	public:
		ODSQueueList(OThread* parent) {
			//create the memory
			q_mem.reset(new ODSQueueListMem(parent));
			
			//initialize the socket callbacks
			q_mem->q_sock.readyReadFunc(bind(&ODSQueueList::readyRead, this));
			q_mem->q_sock.readyWriteFunc(bind(&ODSQueueList::readyWrite, this));
			
			
			
		}
		
		void setHandler(uint16_t packetId, function<void (OByteArray)> cbk) {
			while(q_mem->q_handlers.size() <= packetId) 
				q_mem->q_handlers.push_back(NULL);
			
			q_mem->q_handlers[packetId] = cbk;
		}
		
		
		ODSQueue addQueue(uint16_t handle, int priority) {
			while(q_mem->q_que.size() <= handle) q_mem->q_que.push_back(ODSQueue());
			
			if(q_mem->q_que[handle].isEmpty()) {
				ODSQueue tmp(q_mem.get());
				tmp.setPriority(priority);
				q_mem->q_que[handle] = tmp;
			} else {
				return q_mem->q_que[handle];
			}
		}
		
		ODSQueue addQueue(OString name, int priority = 0) {
			//check to see if the queue already exists
			for(auto i=q_mem->q_que.begin(); i<q_mem->q_que.end(); i++) {
				if(i->q_mem->q_name == name) {
					return *i;
				}
			}
			
			//the queue is not already present so add it
			//first check if there is an empty spot we can use
			ODSQueue tmp(q_mem.get());
			for(auto i=q_mem->q_que.begin(); i<q_mem->q_que.end(); i++) {
				if(i->isEmpty()) {
					*i = tmp;
					tmp.setPriority(priority);
					return tmp;
				}
			}
			
			//there is no empty spot so just add it to the end of the array
			q_mem->q_que.push_back(tmp);
			tmp.setPriority(priority);
			return tmp;
		}
		
		ODSQueue addQueue(int priority) {
			ODSQueue tmp(q_mem.get());
			q_mem->q_que.push_back(tmp);
			tmp.setPriority(priority);
			return tmp;
		}
		
	protected:
		void readyRead() {
			
		}
		
		void readyWrite() {
			
		}
		
		shared_ptr<ODSQueueListMem>			q_mem;
	};
	
	
	
	
	
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

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

class ODataStream;
class ODSQueue;
class ODSMux;

#ifndef ODSPacketType
#define ODSPacketType uint16_t
#endif

#ifndef ODSPacketLength
#define ODSPacketLength	uint32_t
#endif

class ODataStream {
	friend class ODSMux;
protected:
	class ODSQueue {
		friend class ODataStream;
	protected:
		class ODSQueueMem {
		public:
			
			list<OByteArray>					q_que;
			mutex								q_mutex;
			uint16_t							q_priority;
		};
	public:
		ODSQueue() {}
		
		uint16_t priority() {
			if(isInit()) return q_mem->q_priority;
			return 0;
		}
		
		void setPriority(uint16_t p) {
			if(isInit()) q_mem->q_priority = p;
		}
		
		bool isInit() {
			return q_mem.get();
		}
		
		bool isEmpty() {
			if(isInit())
				return !q_mem->q_que.size();
			return false;
		}
		
		void write(OByteArray data) {
			if(isInit()) {
				unique_lock<mutex> locker(q_mem->q_mutex);
				q_mem->q_que.push_back(data);
			}
		}
		
	protected:
		ODSQueue(void* parent) {
			q_mem.reset(new ODSQueueMem());
		}
		
		shared_ptr<ODSQueueMem>				q_mem;
	};
	
	class ODataStreamMem {
		friend class ODataStream;
	protected:
		ODataStreamMem(OThread* parent) : q_sock(parent) {
			q_parent = parent;
			q_readhead = true;
		}
		
		void enableReadyWrite() {
			q_sock.enableReadyWrite();
		}
		
	protected:
		//current packet we are reading in
		ODSPacketType						q_type;
		ODSPacketLength						q_length;
		OByteArray							q_data;
		bool								q_readhead;
		OByteArray							q_head;
		
		//true if we are not finished sending a chunk of data
		bool								q_writing;
		//the packet id of the data we need to finish sending
		uint16_t							q_fpid;
		
		//the offset in the array is the priority, value is the queue
		vector<ODSQueue>					q_pque;
		//the offset in the array is the packetId, value is the queue
		vector<ODSQueue>					q_ique;
		int									q_packetsize;
		OTcpSocket							q_sock;
		OThread*							q_parent;
		//array offset is the id of the packet, value is the function
		//to be called to handle the data
		vector<function<void (OByteArray)>>	q_handlers;
	};
public:
	explicit ODataStream(OThread* parent);
	
	OThread* parent() const;
	void setParent(OThread* parent);
	
	/**	This function sets how a block of data is handled when it is send with
	 *	a given unique identifier. 
	 *	@param packetId The unique ifdentifier of the block of data.
	 *	@param priority The transmission priority of this block of data.
	*/
	void setSendHandler(uint16_t packetId, uint16_t priority = 0);
	
	/**	This function sets a function to be called when a packet with
	 *	a specific unique identifier arrives.
	 *	@param packetId This is the unique identifier.
	 *	@param cbk The function to be bound to the unique identifier.
	*/
	void setRecvHandler(uint16_t packetId, function<void (OByteArray)> cbk);
	
	/**	Get the priority associated with a packet id.
	 *	@return The numeric value for the priority of the packet. Larger number
	 *	specifies higher priority.
	*/
	uint16_t priority(uint16_t packetId);
	
	/**	
	 *	
	*/
	bool sendHandlerIsSet(uint16_t packetId);
	
	/**	
	 *	
	*/
	bool recvHandlerIsSet(uint16_t packetId);
	
	/**	Write some data with a specified unique packet identifier.
	 *	@param packetId The unique identifier that is used by the
	 *	other endpoint to determine how to handle this packet.
	 *	@param data The data that is being transmitted.
	*/
	void write(uint16_t packetId, OByteArray data);
	
	/**	Check if this instance has been initialized. If it has not been
	 *	initialized any of the other function calls with not perform
	 *	any action.
	*/
	bool isInit();
	
	/**	Returns true if all the members in this instance have been
	 *	initialized.
	*/
	bool isEmpty();
	
	/**	Connect to a server with specified address and port. This 
	 *	queue only supports TCP.
	 *	@param addr The address of the host being connected to.
	 *	@param port The port being connected to.
	*/
	bool connect(OString addr, unsigned short port);
	
	/**	This function is overloaded.
	*/
	bool connect(const OSockAddress& addr);
	
	/**	This function is overloaded.
	*/
	bool connect(const OAddressInfo& info);
	
protected:
	void readyRead();
	void readyWrite();
	
	shared_ptr<ODataStreamMem>			q_mem;
};

class ODSMux {
public:
	ODSMux(OThread* parent);
	ODSMux(OThread* parent, OTcpSocket* socket);
	ODSMux(OThread* parent, int fd);
	
	ODataStream connect(OString addr, unsigned short port);
	
	ODataStream connect(const OSockAddress& addr);
	
	ODataStream connect(const OAddressInfo& info);
	
	bool listen(int port, OString ifn = "");
	
	bool listen(const OAddressInfo& info);
	
	bool listen(const OSockAddress&	sock);
	
	int size() const;
	ODataStream operator[](int i);
	
protected:
	OThread*				q_parent;
	OTcpServer				q_serv;
	vector<ODataStream>	q_ques;
	
	//TODO
	//when host connects with unique identifier, use predefined template
	//to initialize the queue callbacks
};

#endif // ODSMUX_H

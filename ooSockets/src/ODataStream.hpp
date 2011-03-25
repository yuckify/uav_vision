#ifndef ODATASTREAM_HPP
#define ODATASTREAM_HPP

#include<OThread.hpp>
#include<OByteArray.hpp>
#include<memory>
#include<functional>
#include<boost/thread/mutex.hpp>

#include<OO.hpp>

using boost::mutex;
using boost::unique_lock;

namespace bst = boost;

using namespace std;

class ODataStreamBase {
public:
	
};

template<bool enableSecurity = false,
		 class PacketLength = uint32_t, 
		 class PacketType = uint16_t>
class ODataStream {
	friend class ODSMux;
protected:
	enum ConfigBit {
		EncryptedPacket = 1,
		ControlPacket = 2,
		
	};

	enum ControlPacket {
		PublicKeyHandshake,
		HostId
	};
	
	class ODSQueue {
		friend class ODataStream;
	protected:
		class ODSQueueMem {
		public:
			ODSQueueMem() {
				q_secure = false;
			}
			
			
			list<OByteArray>					q_que;
			bst::mutex							q_mutex;
			uint16_t							q_priority;
			bool								q_secure;
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
		
		bool isSecure() {
			return q_mem->q_secure;
		}
		
		bool setSecure(bool secure) {
			if(isInit())
				q_mem->q_secure = secure;
		}
		
		bool isEmpty() {
			if(isInit())
				return !q_mem->q_que.size();
			return false;
		}
		
		void write(OByteArray data) {
			if(isInit()) {
				bst::unique_lock<bst::mutex> locker(q_mem->q_mutex);
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
		
#ifdef OO_QT
		ODataStreamMem(QObject* parent) : q_sock(parent) {
			q_parent = parent;
			q_readhead = true;
		}
#endif
		
		void enableReadyWrite() {
			q_sock.enableReadyWrite();
		}
		
	protected:
		//current packet we are reading in
		PacketType							q_type;
		PacketLength						q_length;
		OByteArray							q_data;
		bool								q_readhead;
		OByteArray							q_head;
		
		//true if we are not finished sending a chunk of data
		bool								q_writing;
		//the packet id of the data we need to finish sending
		uint16_t							q_fpid;
		
		OO::Endian							q_end;
		
		//the offset in the array is the priority, value is the queue
		vector<ODSQueue>					q_pque;
		//the offset in the array is the packetId, value is the queue
		vector<ODSQueue>					q_ique;
		int									q_packetsize;
		OTcpSocket							q_sock;
		OThread*							q_parent;
#ifdef OO_QT
		QObject*							q_qparent;
#endif
		//array offset is the id of the packet, value is the function
		//to be called to handle the data
		vector<function<void (OByteArray)>>	q_handlers;
		
		//the encryption key for sending secure information
		Botan::RSA_PublicKey* key;
	};
public:
	explicit ODataStream(OThread* parent) {
		//create the memory
		q_mem.reset(new ODataStreamMem(parent));
		
		//initialize the socket callbacks
		q_mem->q_sock.readyReadFunc(bind(&ODataStream<enableSecurity,
										 PacketLength,
										 PacketType>::
										 readyRead, this));
		q_mem->q_sock.readyWriteFunc(bind(&ODataStream<enableSecurity,
										  PacketLength,
										  PacketType>::
										  readyWrite, this));
										  
	}
	
	void setEndian(OO::Endian end) {
		if(isInit())
			q_mem->q_end = end;
	}
	
	OO::Endian endian() const {
		if(isInit())
			return q_mem->q_end;
		return OO::LittleEndian;
	}
	
	OO::HANDLE fileDescriptor() const {
		if(isInit())
			return q_mem->q_sock.fileDescriptor();
		return NULL;
	}
	
	void setFileDescriptor(OO::HANDLE fd) {
		cout<<"fd: " <<fd <<endl;
		if(isInit())
			q_mem->q_sock.setFileDescriptor(fd);
	}
	
#ifdef OO_QT
	explicit ODataStream(QObject* parent) {
		//create the memory
		q_mem.reset(new ODataStreamMem(parent));
		
		//initialize the socket callbacks
		q_mem->q_sock.readyReadFunc(bind(&ODataStream::readyRead, this));
		q_mem->q_sock.readyWriteFunc(bind(&ODataStream::readyWrite, this));
	}
#endif
	
	OThread* parent() const {
		if(isInit())
			return q_mem->q_parent;
		return NULL;
	}
	
	void setParent(OThread *parent) {
		if(isInit())
			q_mem->q_parent = parent;
	}
	
#ifdef OO_QT
	QObject* qParent() const {
		if(isInit())
			return q_mem->q_qparent;
		return NULL;
	}
	
	void setQParent(QObject* parent) {
		if(isInit())
			q_mem->q_qparent;
	}
#endif
	
	/**	This function sets how a block of data is handled when it is send with
	 *	a given unique identifier. 
	 *	@param packetId The unique ifdentifier of the block of data.
	 *	@param priority The transmission priority of this block of data.
	*/
	void setSendHandler(uint16_t packetId, uint16_t priority = 0) {
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
	
	/**	This function sets a function to be called when a packet with
	 *	a specific unique identifier arrives.
	 *	@param packetId This is the unique identifier.
	 *	@param cbk The function to be bound to the unique identifier.
	*/
	void setRecvHandler(uint16_t packetId, function<void (OByteArray)> cbk) {
		if(isInit()) {
			while(q_mem->q_handlers.size() <= packetId) 
				q_mem->q_handlers.push_back(NULL);
			
			q_mem->q_handlers[packetId] = cbk;
		}
	}
	
	void setSecurity(uint16_t packetId, bool secure) {
		if(isInit()) {
			if(q_mem->q_ique.size() > packetId) {
				q_mem->q_ique[packetId].setSecure(secure);
			}
		}
	}
	
	/**	Get the priority associated with a packet id.
	 *	@return The numeric value for the priority of the packet. Larger number
	 *	specifies higher priority.
	*/
	uint16_t priority(uint16_t packetId) {
		if(isInit()) {
			if(q_mem->q_ique.size() > packetId)
				return q_mem->q_ique[packetId].priority();
		}
		return 0;
	}
	
	/**	
	 *	
	*/
	bool sendHandlerIsSet(uint16_t packetId) {
		if(isInit()) {
			if(q_mem->q_ique.size() > packetId)
				return q_mem->q_ique[packetId].isInit();
		}
		return false;
	}
	
	/**	
	 *	
	*/
	bool recvHandlerIsSet(uint16_t packetId) {
		if(isInit()) {
			return q_mem->q_handlers[packetId] != NULL;
		}
		return false;
	}
	
	/**	Write some data with a specified unique packet identifier.
	 *	@param packetId The unique identifier that is used by the
	 *	other endpoint to determine how to handle this packet.
	 *	@param data The data that is being transmitted.
	*/
	void write(uint16_t packetId, OByteArray data) {
		if(isInit()) {
			if(q_mem->q_ique.size() > packetId) {
				q_mem->q_ique[packetId].write(data);
				q_mem->q_sock.enableReadyWrite();
			}
		}
	}
	
	/**	Check if this instance has been initialized. If it has not been
	 *	initialized any of the other function calls with not perform
	 *	any action.
	*/
	bool isInit() {
		return q_mem.get();
	}
	
	/**	Returns true if all the members in this instance have been
	 *	initialized.
	*/
	bool isEmpty() {
		if(isInit()) {
			return !q_mem->q_pque.size() && !q_mem->q_ique.size() &&
					!q_mem->q_handlers.size() && q_mem->q_sock.isEmpty();
		}
		return false;
	}
	
	/**	Connect to a server with specified address and port. This 
	 *	queue only supports TCP.
	 *	@param addr The address of the host being connected to.
	 *	@param port The port being connected to.
	*/
	bool connect(OString addr, unsigned short port) {
		return q_mem->q_sock.connect(addr, port);
	}
	
	/**	This function is overloaded.
	*/
	bool connect(const OSockAddress& addr) {
		return q_mem->q_sock.connect(addr);
	}
	
	/**	This function is overloaded.
	*/
	bool connect(const OAddressInfo& info) {
		return q_mem->q_sock.connect(info);
	}
	
protected:
	void readyRead() {
		cout<<"ready read" <<endl;
		do {
			cout<<"do" <<endl;
			//first check if we need to read in the header for the next packet
			if(q_mem->q_readhead) {
				cout<<"reading" <<endl;
				//read in the header which is the size of the length field
				///minus the amount that has already been read in
				q_mem->q_sock.read(q_mem->q_head, sizeof(PacketLength) - 
												q_mem->q_head.size());
				
				cout<<"read:"  <<q_mem->q_head.size() <<endl;
				
				//readhead = true if head.size() != sizeof(header information)
				//this bool specifies if we have read in the whole header or
				//not, true (have not read in header) false (have read in header)
				q_mem->q_readhead = q_mem->q_head.size() != sizeof(PacketLength);
				
				cout<<"readhead: " <<q_mem->q_readhead <<endl;
				
				//we have not finished reading in the head, so return and wait
				//for more data
				if(q_mem->q_readhead) return;
				
				for(auto i=q_mem->q_head.begin(); i<q_mem->q_head.end(); i++) {
					cout<<(int)*((unsigned char*)i) <<" ";
				}cout<<endl;
				
				//deserialize the header information
				q_mem->q_head.seek(0);
				q_mem->q_head>>(q_mem->q_length);
				cout<<"len: " <<q_mem->q_length <<endl;
				
				q_mem->q_head.clear();
			}
			
			//read in some data, make sure the new data gets appended
			//to the block of old data
			q_mem->q_sock.read(q_mem->q_data, q_mem->q_length - q_mem->q_data.size());
			
			cout<<"ds: " <<q_mem->q_data.size() <<endl;
			if(q_mem->q_data.size() == q_mem->q_length) {
				//read out the packet type
				q_mem->q_data>>(q_mem->q_type);
				cout<<"type: " <<(int)q_mem->q_type <<endl;
				
				//if a function is set for this packet signature, call it
				if(q_mem->q_handlers[q_mem->q_type])
					q_mem->q_handlers[q_mem->q_type](q_mem->q_data);
				
				//we are done with this data
				q_mem->q_data.clear();
				q_mem->q_readhead = true;
			}
			
		} while(q_mem->q_data.size() == q_mem->q_length);
	}
	
	void readyWrite() {
		for(auto i=q_mem->q_pque.rbegin(); i<q_mem->q_pque.rend(); i++) {
			if(i->isInit()) {
				//if this queue has been initialized handle the data
				if(i->q_mem->q_que.size()) {
					//if this queue has data write it
					q_mem->q_sock.write(i->q_mem->q_que.front());
					//check if this queue is empty, if so pop it
					if(!i->q_mem->q_que.front().dataLeft()) {
						i->q_mem->q_que.pop_front();
					}
				}
			}
		}
	}
	
	shared_ptr<ODataStreamMem>			q_mem;
};

#endif // ODATASTREAM_HPP

#ifndef ODATASTREAM_HPP
#define ODATASTREAM_HPP

#include<OThread.hpp>
#include<OByteArray.hpp>
#include<memory>
#include<functional>
#include<boost/thread/mutex.hpp>
#include<sys/types.h>
#include<assert.h>

#include<OTcpSocket.hpp>
#include<OO.hpp>
#include<OPipe.hpp>

using boost::mutex;
using boost::unique_lock;

namespace bst = boost;

using namespace std;

class ODataStreamBase {
public:
	
	virtual void write(uint16_t type, OByteArray data) = 0;
	virtual bool connected() = 0;
	
};

//config byte
//lsb
//<control packet>
//<encryped>
//
//msb
//
////DataChunk
//
//
//
//
template<class PacketLength = uint32_t, 
		 class PacketType = uint16_t,
		 OO::Endian useEndian = OO::LittleEndian,
		 bool enableConfig = false,
		 bool enableSecurity = false>
							   class ODataStream : public ODataStreamBase {
	friend class ODSMux;
protected:
	enum ConfigBit {
		EncryptedPacket = 1,
		ControlPacket = 2,
		EndPacket = 4,
		
	};

	enum ControlPacket {
		//<length><config><type><data>
		PublicKeyHandshake,
		
		//<length><config><type><end><data>
		DataChunk
	};
	
	enum WriteState {
		WriteInit,
		WriteHigh,
		WriteFinish,
		WriteLow
	};
	
	struct Packet {
		Packet() {}

		Packet(uint16_t t, OByteArray d) {
			q_data = d;
			q_type = t;
		}

		OByteArray	q_data;
		uint16_t	q_type;
	};
	
	class ODSOutQueue {
	protected:
		class ODSOutQueueMem {
		public:
			ODSOutQueueMem() {
				q_secure = false;
			}
			
			list<Packet>						q_que;
			bst::mutex							q_mutex;
			uint16_t							q_priority;
			bool								q_secure;
		};
	public:
		shared_ptr<ODSOutQueueMem>				q_mem;
		
		ODSOutQueue() : q_mem(new ODSOutQueueMem()) {}
		
		uint16_t priority() {
			return q_mem->q_priority;
			return 0;
		}
		
		void setPriority(uint16_t p) {
			q_mem->q_priority = p;
		}
		
		bool isSecure() {
			return q_mem->q_secure;
		}
		
		void setSecure(bool secure) {
			q_mem->q_secure = secure;
		}
		
		bool hasData() {
			return q_mem->q_que.size();
		}
		
		void write(uint16_t type, OByteArray data) {
			bst::unique_lock<bst::mutex> locker(q_mem->q_mutex);
			q_mem->q_que.push_back(Packet(type, data));
		}
	};
	
	class ODataStreamMem {
		friend class ODataStream;
	protected:
		ODataStreamMem(OThread* parent) : q_sock(parent) {
			q_readhead = true;
			q_writeState = WriteInit;
		}
		
#ifdef OO_QT
		ODataStreamMem(QObject* parent) : q_sock(parent) {
			q_readhead = true;
			q_writeState = WriteInit;
		}
#endif
		
		void enableReadyWrite() {
			q_sock.enableReadyWrite();
		}
		
	protected:
		//current packet we are reading in
		PacketType							q_type;
		PacketLength						q_length;
		uint8_t								q_config;
		bool								q_readhead;
		OByteArray							q_head;
		
		//the packet we need to finish sending
		Packet								q_writepacket;
//		unsigned							q_current;
		WriteState							q_writeState;
		function<void ()>					writeFuns[4];
		
		boost::mutex						q_writetex;
		
		//receiver buffers, index value is the packetId
		vector<OByteArray>					q_recvBuff;
		
		pthread_t							q_creator;
		OPipe								q_activity;
		
		OO::Endian							q_end;
		
		//the offset in the array is the priority, value is the queue
		vector<ODSOutQueue>					q_pque;
		//the offset in the array is the packetId, value is the queue
		vector<ODSOutQueue>					q_ique;
		int									q_packetsize;
		OTcpSocket							q_sock;
		function<void ()>					q_disconnect;
		//array offset is the id of the packet, value is the function
		//to be called to handle the data
		vector<function<void (OByteArray)>>	q_handlers;
		
		vector<function<void (OByteArray)>> q_controlHandlers;
		
		//the encryption key for sending secure information
#ifdef OO_BOTAN
		Botan::RSA_PublicKey* key;
#endif
	};

public:
	explicit ODataStream(OThread* parent) {
		cout<<"oos ds" <<endl;
		//create the memory
		q_mem.reset(new ODataStreamMem(parent));
		
		//initialize the write state functions
		q_mem->writeFuns[WriteInit] = bind(&ODataStream<PacketLength,
							PacketType, useEndian,
							enableConfig, enableSecurity>::
							writeInit, this);
		q_mem->writeFuns[WriteHigh] = bind(&ODataStream<PacketLength,
							PacketType, useEndian,
							enableConfig, enableSecurity>::
							writeHigh, this);
		q_mem->writeFuns[WriteFinish] = bind(&ODataStream<PacketLength,
							PacketType, useEndian,
							enableConfig, enableSecurity>::
							writeFinish, this);
		q_mem->writeFuns[WriteLow] = bind(&ODataStream<PacketLength,
							PacketType, useEndian,
							enableConfig, enableSecurity>::
							writeLow, this);
		
		//initialize the socket callbacks
		q_mem->q_sock.readyReadFunc(bind(&ODataStream<PacketLength,
										 PacketType, useEndian,
										 enableConfig, enableSecurity>::
										 readyRead, this));
		q_mem->q_sock.readyWriteFunc(bind(&ODataStream<PacketLength,
										  PacketType, useEndian,
										  enableConfig, enableSecurity>::
										  readyWrite, this));
		q_mem->q_sock.disconnectFunc(bind(&ODataStream<PacketLength,
										  PacketType, useEndian,
										  enableConfig, enableSecurity>::
										  sockDisconnected, this));
		
		//initialize the pipe callback
		q_mem->q_activity.readFunc(bind(&ODataStream<PacketLength,
											 PacketType, useEndian,
											 enableConfig, enableSecurity>::
											pipeRead, this));
		q_mem->q_creator = pthread_self();
		q_mem->q_activity.parent(parent);
		
		q_mem->q_packetsize = 5000;
		
		//make sure the memory buffers know what endian we are using
		q_mem->q_head.setEndian(useEndian);
//		q_mem->q_data.setEndian(useEndian);
	}
	
#ifdef OO_QT
	explicit ODataStream(QObject* parent) {
		//create the memory
		q_mem.reset(new ODataStreamMem(parent));
		
		//initialize the write state functions
		q_mem->writeFuns[WriteInit] = bind(&ODataStream<PacketLength,
							PacketType, useEndian,
							enableConfig, enableSecurity>::
							writeInit, this);
		q_mem->writeFuns[WriteHigh] = bind(&ODataStream<PacketLength,
							PacketType, useEndian,
							enableConfig, enableSecurity>::
							writeHigh, this);
		q_mem->writeFuns[WriteFinish] = bind(&ODataStream<PacketLength,
							PacketType, useEndian,
							enableConfig, enableSecurity>::
							writeFinish, this);
		q_mem->writeFuns[WriteLow] = bind(&ODataStream<PacketLength,
							PacketType, useEndian,
							enableConfig, enableSecurity>::
							writeLow, this);
		
		//initialize the socket callbacks
		q_mem->q_sock.readyReadFunc(bind(&ODataStream<PacketLength,
										 PacketType, useEndian,
										 enableConfig, enableSecurity>::
										 readyRead, this));
		q_mem->q_sock.readyWriteFunc(bind(&ODataStream<PacketLength,
										  PacketType, useEndian,
										  enableConfig, enableSecurity>::
										  readyWrite, this));
		q_mem->q_sock.disconnectFunc(bind(&ODataStream<PacketLength,
										  PacketType, useEndian,
										  enableConfig, enableSecurity>::
										  sockDisconnected, this));
		
		//initialize the pipe callback
		q_mem->q_activity.readFunc(bind(&ODataStream<PacketLength,
											 PacketType, useEndian,
											 enableConfig, enableSecurity>::
											pipeRead, this));
		q_mem->q_creator = pthread_self();
		
		q_mem->q_packetsize = 5000;
		
		//make sure the memory buffers know what endian we are using
		q_mem->q_head.setEndian(useEndian);
//		q_mem->q_data.setEndian(useEndian);
	}
#endif
	
	ODataStream(ODataStream& other) {
		q_mem = other.q_mem;
	}
	
	void setEndian(OO::Endian end) {
		q_mem->q_end = end;
	}
	
	OO::Endian endian() const {
		return q_mem->q_end;
	}
	
	OO::HANDLE fileDescriptor() const {
			return q_mem->q_sock.fileDescriptor();
	}
	
	void setFileDescriptor(OO::HANDLE fd) {
		q_mem->q_sock.setFileDescriptor(fd);
	}
	
	OThread* parent() const {
		return q_mem->q_parent;
	}
	
	void setParent(OThread *parent) {
		q_mem->q_parent = parent;
		q_mem->q_sock.setParent(parent);
	}
	
#ifdef OO_QT
	QObject* qParent() const {
		return q_mem->q_qparent;
	}
	
	void setQParent(QObject* parent) {
		q_mem->q_qparent = parent;
		q_mem->q_sock.setParent(parent);
	}
#endif
	
	bool connected() {
		return q_mem->q_sock.connected();
	}
	
	void setHandler(uint16_t packetId, uint16_t priority, 
					function<void (OByteArray)> cbk) {
		setRecvHandler(packetId, cbk);
		setSendHandler(packetId, priority);
	}
	
	/**	This function sets how a block of data is handled when it is send with
	 *	a given unique identifier. 
	 *	@param packetId The unique ifdentifier of the block of data.
	 *	@param priority The transmission priority of this block of data.
	*/
	void setSendHandler(uint16_t packetId, uint16_t priority = 0) {
		while(q_mem->q_ique.size() <= packetId) 
			q_mem->q_ique.push_back(ODSOutQueue());
		
		while(q_mem->q_pque.size() <= priority)
			q_mem->q_pque.push_back(ODSOutQueue());
		
		q_mem->q_pque[priority].setPriority(priority);
		
		//set that we want to use this specific queue with this packetId
		q_mem->q_ique[packetId] = q_mem->q_pque[priority];
	}
	
	/**	This function sets a function to be called when a packet with
	 *	a specific unique identifier arrives.
	 *	@param packetId This is the unique identifier.
	 *	@param cbk The function to be bound to the unique identifier.
	*/
	void setRecvHandler(uint16_t packetId, function<void (OByteArray)> cbk) {
		while(q_mem->q_handlers.size() <= packetId) 
			q_mem->q_handlers.push_back(NULL);
		
		q_mem->q_handlers[packetId] = cbk;
	}
	
	void setSecurity(uint16_t packetId, bool secure) {
		if(q_mem->q_ique.size() > packetId) {
			q_mem->q_ique[packetId].setSecure(secure);
		}
	}
	
	void setDisconnectSig(function<void ()> cbk) {
		q_mem->q_disconnect = cbk;
	}
	
	/**	Get the priority associated with a packet id.
	 *	@return The numeric value for the priority of the packet. Larger number
	 *	specifies higher priority.
	*/
	uint16_t priority(uint16_t packetId) {
		if(q_mem->q_ique.size() > packetId)
			return q_mem->q_ique[packetId].priority();
		return 0;
	}
	
	/**	
	 *	
	*/
	bool sendHandlerIsSet(uint16_t packetId) {
		if(q_mem->q_ique.size() > packetId)
			return q_mem->q_ique[packetId].isInit();
	}
	
	/**	
	 *	
	*/
	bool recvHandlerIsSet(uint16_t packetId) {
		return q_mem->q_handlers[packetId] != NULL;
	}
	
	/**	Write some data with a specified unique packet identifier.
	 *	@param packetId The unique identifier that is used by the
	 *	other endpoint to determine how to handle this packet.
	 *	@param data The data that is being transmitted.
	*/
	void write(uint16_t packetId, OByteArray data) {
		boost::unique_lock<boost::mutex> locker(q_mem->q_writetex);
		data.seek(0);
		
		if(q_mem->q_ique.size() <= packetId) return;
		
		q_mem->q_ique[packetId].write(packetId, data);
		q_mem->q_sock.enableReadyWrite();
		
		//if the thread writing the data is not the one that created
		//this ODataStream then pipe some data over to the one that did make
		//this ODatastream
		OByteArray d("d", 1);
		q_mem->q_activity.write(d);
	}
	
	/**	Returns true if all the members in this instance have been
	 *	initialized.
	*/
	bool isEmpty() {
		return !q_mem->q_pque.size() && !q_mem->q_ique.size() &&
				!q_mem->q_handlers.size() && q_mem->q_sock.isEmpty();
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
		do {
			//first check if we need to read in the header for the next packet
			if(q_mem->q_readhead) {
				//read in the header which is the size of the length field
				//and the other fields
				///minus the amount that has already been read in
				if(enableConfig) {
					static const int header_size = sizeof(PacketLength) +
												   sizeof(PacketType) +
												   sizeof(uint8_t);
					
					q_mem->q_sock.read(q_mem->q_head, header_size -
													q_mem->q_head.size());
					
					//check if we need to read in more header information
					q_mem->q_readhead = q_mem->q_head.size() != header_size;
				} else {
					static const int header_size = sizeof(PacketLength) +
												   sizeof(PacketType);
					
					q_mem->q_sock.read(q_mem->q_head, header_size - 
													q_mem->q_head.size());
					
					//check if we need to read in more header information
					q_mem->q_readhead = q_mem->q_head.size() != header_size;
				}
				
				//we have not finished reading in the head, so return and wait
				//for more data
				if(q_mem->q_readhead) return;
				
				//deserialize the header information
				q_mem->q_head.seek(0);
				q_mem->q_head>>(q_mem->q_length);
				
				//if we are using he config byte deserialize it
				if(enableConfig) {
					q_mem->q_head>>(q_mem->q_config);
				}
				
				//read the packet type
				q_mem->q_head>>(q_mem->q_type);
				
				//we are done with the header buffer so clear it
				q_mem->q_head.clear();
			}
			
			//make sure we have a buffer allocated for the packet
			while(q_mem->q_type >= q_mem->q_recvBuff.size()) {
				q_mem->q_recvBuff.push_back(OByteArray());
				q_mem->q_recvBuff.back().setEndian(useEndian);
			}
			
			//get the curret buffer we are working with
			OByteArray& data = q_mem->q_recvBuff[q_mem->q_type];
			
			//read in some data, make sure the new data gets appended
			//to the block of old data
			q_mem->q_sock.read(data, q_mem->q_length - data.size() - 
							   sizeof(PacketType) -
							   sizeof(uint8_t));
			
			if(data.size() >= (q_mem->q_length-sizeof(PacketType)-sizeof(uint8_t))) {
				//read out the packet type
				data.seek(0);
				
				if(!(q_mem->q_config & EndPacket)) {
					return;
				}
				
				//if a function is set for this packet signature, call it
				if(q_mem->q_handlers.size() > q_mem->q_type) {
					if(q_mem->q_handlers[q_mem->q_type]) {
						q_mem->q_handlers[q_mem->q_type](data);
					}
				}
				
				//we are done with this data
				data.clear();
				q_mem->q_readhead = true;
			}
			
		} while(q_mem->q_recvBuff[q_mem->q_type].size() == q_mem->q_length);
	}
	
	void writeInit() {
		q_mem->q_writeState = WriteHigh;
		writeHigh();
	}
	
	void writeHigh() {
		for(auto i=q_mem->q_pque.rbegin(); i<q_mem->q_pque.rend(); i++) {
			//we have found the first queue, check if it contains any chunks of
			//data to write
			if(i->hasData()) {
				//prepare the header for the packet
				OByteArray head;
				OByteArray& data = i->q_mem->q_que.front().q_data;
				
				//setup the variable that make up the header for this packet
				PacketLength length = 0;
				uint8_t config = EndPacket;
				PacketType type = i->q_mem->q_que.front().q_type;
				head<<length <<config <<type;
				
				head.seek(0);
				length = data.size() + head.size() - sizeof(PacketLength);
				head<<length;
				
				//assume the whole header will be written...
				head.seek(0);
				unsigned head_written = q_mem->q_sock.write(head);
				
				//this throws if not all the data in the head gets written
				assert(head_written == head.size());
				
				//write the payload
				q_mem->q_sock.write(data);
				
				//check if we wrote all of the payload or just most of it
				if(!data.dataLeft()) {
					//we wrote all this packet pop it from the queue
					i->q_mem->q_que.pop_front();
					
					//we are done here so reset to init
					q_mem->q_writeState = WriteInit;
					return;
				}
				
				//store the remaining packet to be written when more room in
				//the network stack becomes available
				q_mem->q_writepacket = i->q_mem->q_que.front();
				i->q_mem->q_que.pop_front();
				
				q_mem->q_writeState = WriteFinish;
				
				return;
			} else {
				//else change state to write low
				q_mem->q_writeState = WriteLow;
				writeLow();
				return;
			}
		}
		
		q_mem->q_writeState = WriteLow;
		writeLow();
	}
	
	void writeLow() {
		//start with the first packetwe looked at last after a call to write
		//the highest priority packet
		for(auto i=q_mem->q_pque.rbegin()+1; i<q_mem->q_pque.rend(); i++) {
			if(i->hasData()) {
				//prepare the header for the packet
				OByteArray head;
				OByteArray& data = i->q_mem->q_que.front().q_data;
				
				//compute how much of the payload will be written
				uint32_t datalen;
				if(data.size() > q_mem->q_packetsize)	datalen = q_mem->q_packetsize;
					else								datalen = data.size();
				
				//setup the variables that make up the header for this packet
				PacketLength length = 0;
				uint8_t config = 0;
				if(data.dataLeft() == datalen) config |= EndPacket;
				PacketType type = i->q_mem->q_que.front().q_type;
				head<<length <<config <<type;
				
				head.seek(0);
				length = datalen + head.size() - sizeof(PacketLength);
				head<<length;
				
				//assume the whole header will be written...
				head.seek(0);
				unsigned head_written = q_mem->q_sock.write(head);
				
				//this throws if not all the data in the head gets written
				assert(head_written == head.size());
				
				//write the payload
				q_mem->q_sock.write(data, datalen);
				
				//check if we wrote all of the payload or just most of it
				if(!data.dataLeft()) {
					//we wrote all this packet pop it from the queue
					i->q_mem->q_que.pop_front();
					
					//we are done here so reset to init
					q_mem->q_writeState = WriteInit;
					return;
				}
				
				//store the remaining packet to be written when more room in
				//the network stack becomes available
				q_mem->q_writepacket = i->q_mem->q_que.front();
				i->q_mem->q_que.pop_front();
				
				q_mem->q_writeState = WriteFinish;
				
				return;
			}
		}
		q_mem->q_writeState = WriteInit;
	}
	
	void writeFinish() {
		//write the remaining data
//		cout<<"finish data left: " <<q_mem->q_writepacket.q_data.dataLeft() <<endl;
//		cout<<"write finished: " <<q_mem->q_sock.write(q_mem->q_writepacket.q_data) <<endl;
		
		//check if there is any remaining data left to be written
		if(!q_mem->q_writepacket.q_data.dataLeft()) {
			q_mem->q_writeState = WriteInit;
		}//else stay in this state finish writing data
	}
	
	void readyWrite() {
		boost::unique_lock<boost::mutex> locker(q_mem->q_writetex);
		q_mem->writeFuns[q_mem->q_writeState]();
	}
	
	void pipeRead() {
		q_mem->q_sock.enableReadyWrite();
		OByteArray data = q_mem->q_activity.readAll();
	}
	
	void sockDisconnected() {
		//clear all the buffers
		q_mem->q_readhead = true;
		q_mem->q_head.clear();
		for(auto i=q_mem->q_pque.begin(); i<q_mem->q_pque.end(); i++) {
			i->q_mem->q_que.clear();
		}
		for(auto i=q_mem->q_recvBuff.begin(); i<q_mem->q_recvBuff.end(); i++) {
			i->clear();
		}
		
		if(q_mem->q_disconnect)
			q_mem->q_disconnect();
	}
	
	shared_ptr<ODataStreamMem>			q_mem;
};

#endif // ODATASTREAM_HPP

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
		 class PacketType = uint8_t,
		 bool enableConfig = true,
		 bool enableSecurity = false>
							   class ODataStream {
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

		Packet(PacketType t, OByteArray d) {
			q_data = d;
			q_type = t;
		}

		OByteArray	q_data;
		PacketType	q_type;
	};
	
	class ODSOutQueue {
	protected:
		class ODSOutQueueMem {
		public:
			ODSOutQueueMem() {
				q_secure = false;
				q_writing = false;
			}
			
			list<Packet>						q_que;
			bst::mutex							q_mutex;
			uint16_t							q_priority;
			bool								q_secure;
			bool								q_writing;
		};
	public:
		shared_ptr<ODSOutQueueMem>				q_mem;
		
		ODSOutQueue() : q_mem(new ODSOutQueueMem()) {}
		
		inline uint16_t priority() {
			return q_mem->q_priority;
			return 0;
		}
		
		inline void lock() {
			q_mem->q_mutex.lock();
		}
		
		inline void unlock() {
			q_mem->q_mutex.unlock();
		}
		
		inline void setPriority(uint16_t p) {
			q_mem->q_priority = p;
		}
		
		inline bool isSecure() {
			return q_mem->q_secure;
		}
		
		inline void setSecure(bool secure) {
			q_mem->q_secure = secure;
		}
		
		inline bool hasData() {
			return q_mem->q_que.size();
		}
		
		inline void write(PacketType type, OByteArray data) {
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
		function< void () >					writeFuns[4];
		
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
		unsigned							q_packetsize;
		OTcpSocket							q_sock;
		function< void () >					q_disconnect;
		//array offset is the id of the packet, value is the function
		//to be called to handle the data
		vector< function< void (OByteArray) > >	q_handlers;
		
		vector< function< void (OByteArray) > > q_controlHandlers;
		
		//the encryption key for sending secure information
#ifdef OO_BOTAN
		Botan::RSA_PublicKey* key;
#endif
	};

public:
	explicit ODataStream(OThread* parent) {
		//create the memory
		q_mem.reset(new ODataStreamMem(parent));
		
		//initialize the write state functions
		q_mem->writeFuns[WriteInit] = bind(&ODataStream<PacketLength,
							PacketType, enableConfig, enableSecurity>::
							writeInit, this);
		q_mem->writeFuns[WriteHigh] = bind(&ODataStream<PacketLength,
							PacketType, enableConfig, enableSecurity>::
							writeHigh, this);
		q_mem->writeFuns[WriteFinish] = bind(&ODataStream<PacketLength,
							PacketType, enableConfig, enableSecurity>::
							writeFinish, this);
		q_mem->writeFuns[WriteLow] = bind(&ODataStream<PacketLength,
							PacketType, enableConfig, enableSecurity>::
							writeLow, this);
		
		//initialize the socket callbacks
		q_mem->q_sock.readyReadFunc(bind(&ODataStream<PacketLength,
										 PacketType, enableConfig, enableSecurity>::
										 readyRead, this));
		q_mem->q_sock.readyWriteFunc(bind(&ODataStream<PacketLength,
										  PacketType, enableConfig, enableSecurity>::
										  readyWrite, this));
		q_mem->q_sock.disconnectFunc(bind(&ODataStream<PacketLength,
										  PacketType, enableConfig, enableSecurity>::
										  sockDisconnected, this));
		
		//initialize the pipe callback
		q_mem->q_activity.readFunc(bind(&ODataStream<PacketLength,
											 PacketType, enableConfig, enableSecurity>::
											pipeRead, this));
		q_mem->q_creator = pthread_self();
		q_mem->q_activity.parent(parent);
		
		q_mem->q_packetsize = 5000;
		
		//make sure the memory buffers know what endian we are using
		setEndian(OO::LittleEndian);
		q_mem->q_head.setEndian(endian());
	}
	
#ifdef OO_QT
	explicit ODataStream(QObject* parent) {
		//create the memory
		q_mem.reset(new ODataStreamMem(parent));
		
		//initialize the write state functions
		q_mem->writeFuns[WriteInit] = bind(&ODataStream<PacketLength,
							PacketType, enableConfig, enableSecurity>::
							writeInit, this);
		q_mem->writeFuns[WriteHigh] = bind(&ODataStream<PacketLength,
							PacketType, enableConfig, enableSecurity>::
							writeHigh, this);
		q_mem->writeFuns[WriteFinish] = bind(&ODataStream<PacketLength,
							PacketType, enableConfig, enableSecurity>::
							writeFinish, this);
		q_mem->writeFuns[WriteLow] = bind(&ODataStream<PacketLength,
							PacketType, enableConfig, enableSecurity>::
							writeLow, this);
		
		//initialize the socket callbacks
		q_mem->q_sock.readyReadFunc(bind(&ODataStream<PacketLength,
										 PacketType, enableConfig, enableSecurity>::
										 readyRead, this));
		q_mem->q_sock.readyWriteFunc(bind(&ODataStream<PacketLength,
										  PacketType, enableConfig, enableSecurity>::
										  readyWrite, this));
		q_mem->q_sock.disconnectFunc(bind(&ODataStream<PacketLength,
										  PacketType, enableConfig, enableSecurity>::
										  sockDisconnected, this));
		
		//initialize the pipe callback
		q_mem->q_activity.readFunc(bind(&ODataStream<PacketLength,
											 PacketType, enableConfig, enableSecurity>::
											pipeRead, this));
		q_mem->q_creator = pthread_self();
		
		q_mem->q_packetsize = 5000;
		
		//make sure the memory buffers know what endian we are using
		setEndian(OO::LittleEndian);
		q_mem->q_head.setEndian(endian());
	}
#endif
	
	ODataStream(ODataStream& other) {
		q_mem = other.q_mem;
	}
	
	void setEndian(OO::Endian end) {
		if(q_mem)
			q_mem->q_end = end;
	}
	
	OO::Endian endian() const {
		return q_mem->q_end;
	}
	
	OTcpSocket& socket() {
		return q_mem->q_sock;
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
	
	void setHandler(PacketType packetId, uint16_t priority, 
					function< void (OByteArray) > cbk) {
		setRecvHandler(packetId, cbk);
		setSendHandler(packetId, priority);
	}
	
	/**	This function sets how a block of data is handled when it is send with
	 *	a given unique identifier. 
	 *	@param packetId The unique ifdentifier of the block of data.
	 *	@param priority The transmission priority of this block of data.
	*/
	void setSendHandler(PacketType packetId, uint16_t priority = 0) {
		while(q_mem->q_ique.size() <= packetId) 
			q_mem->q_ique.push_back(ODSOutQueue());
		
		while(q_mem->q_pque.size() <= priority)
			q_mem->q_pque.push_back(ODSOutQueue());
		
		q_mem->q_pque[priority].setPriority(priority);
		
		//set that we want to use this specific queue with this packetId
		q_mem->q_ique[packetId] = q_mem->q_pque[priority];
		q_mem->q_ique[packetId].q_mem->q_writing = true;
	}
	
	/**	This function sets a function to be called when a packet with
	 *	a specific unique identifier arrives.
	 *	@param packetId This is the unique identifier.
	 *	@param cbk The function to be bound to the unique identifier.
	*/
	void setRecvHandler(PacketType packetId, function< void (OByteArray) > cbk) {
		while(q_mem->q_handlers.size() <= packetId) 
			q_mem->q_handlers.push_back(function< void (OByteArray) >());
		
		q_mem->q_handlers[packetId] = cbk;
	}
	
	void setSecurity(uint16_t packetId, bool secure) {
		if(q_mem->q_ique.size() > packetId) {
			q_mem->q_ique[packetId].setSecure(secure);
		}
	}
	
	void setDisconnectHandler(function< void () > cbk) {
		q_mem->q_disconnect = cbk;
	}
	
	void clearHandlers() {
		q_mem->q_handlers.clear();
		q_mem->q_disconnect = NULL;
	}
	
	/**	Get the priority associated with a packet id.
	 *	@return The numeric value for the priority of the packet. Larger number
	 *	specifies higher priority.
	*/
	uint16_t priority(PacketType packetId) {
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
	bool recvHandlerIsSet(PacketType packetId) {
		return q_mem->q_handlers[packetId] != NULL;
	}
	
	/**	Write some data with a specified unique packet identifier.
	 *	@param packetId The unique identifier that is used by the
	 *	other endpoint to determine how to handle this packet.
	 *	@param data The data that is being transmitted.
	*/
	void write(PacketType packetId, OByteArray data) {
		data.seek(0);
		
		if(q_mem->q_ique.size() <= packetId) return;
		
		boost::mutex& tex = q_mem->q_ique[packetId].q_mem->q_mutex;
		boost::unique_lock<boost::mutex> locker(tex);
		if(q_mem->q_ique[packetId].q_mem->q_writing) {
			q_mem->q_ique[packetId].write(packetId, data);
			q_mem->q_sock.enableReadyWrite();
			
			//if the thread writing the data is not the one that created
			//this ODataStream then pipe some data over to the one that did make
			//this ODatastream
//			if(q_mem->q_creator != pthread_self()) {
				OByteArray d("d", 1);
				q_mem->q_activity.write(d);
//			}
		}
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
//		cout<<"read" <<endl;
		do {
			//first check if we need to read in the header for the next packet
			if(q_mem->q_readhead) {
				//read in the header which is the size of the length field
				//and the other fields
				///minus the amount that has already been read in
				if(enableConfig) {
					static const unsigned header_size = sizeof(PacketLength) +
												   sizeof(PacketType) +
												   sizeof(uint8_t);
					
					if(q_mem->q_sock.read(q_mem->q_head, header_size -
										  q_mem->q_head.size()) < 0) {
						return;
					}
					
					//check if we need to read in more header information
					q_mem->q_readhead = q_mem->q_head.size() != header_size;
				} else {
					static const unsigned header_size = sizeof(PacketLength) +
												   sizeof(PacketType);
					
					if(q_mem->q_sock.read(q_mem->q_head, header_size - 
										  q_mem->q_head.size()) < 0) {
						return;
					}
					
					//check if we need to read in more header information
					q_mem->q_readhead = q_mem->q_head.size() != header_size;
				}
				
//				cout<<"read:"  <<q_mem->q_head.size() <<endl;
				
//				for(int i=0; i<q_mem->q_head.size(); i++) {
//					cout<<(int)q_mem->q_head.data()[i] <<" ";
//				} cout<<endl;
				
//				cout<<"avail: " <<q_mem->q_sock.available() <<endl;
				
				//we have not finished reading in the head, so return and wait
				//for more data
//				cout<<"readhead: " <<q_mem->q_readhead <<endl;
				if(q_mem->q_readhead) return;
				
				//deserialize the header information
				q_mem->q_head.seek(0);
				q_mem->q_head>>(q_mem->q_length);
				
				//if we are using the config byte deserialize it
				if(enableConfig) {
					q_mem->q_head>>(q_mem->q_config);
				}
				
				//read the packet type
				q_mem->q_head>>(q_mem->q_type);
				
//				cout<<"l: " <<q_mem->q_length <<" c: " <<(int)q_mem->q_config 
//						<<" t: " <<(int)q_mem->q_type <<endl;
				
				//we are done with the header buffer so clear it
				q_mem->q_head.clear();
			}
			
			//make sure we have a buffer allocated for the packet
			while(q_mem->q_type >= q_mem->q_recvBuff.size()) {
				q_mem->q_recvBuff.push_back(OByteArray());
				q_mem->q_recvBuff.back().setEndian(endian());
			}
			
//			cout<<"t: " <<(int)q_mem->q_type <<" l: " <<q_mem->q_length <<endl;
			
			//get the curret buffer we are working with
			OByteArray& data = q_mem->q_recvBuff[q_mem->q_type];
			
			//read in some data, make sure the new data gets appended
			//to the block of old data
//			cout<<"read payload" <<endl;
//			cout<<"ba: " <<q_mem->q_sock.available() <<endl;
//			cout<<"reading: " <<q_mem->q_length - data.size() - 
//					sizeof(PacketType) -
//					sizeof(uint8_t) <<endl;
//			cout<<"td: " <<data.tell() <<endl;
			if(q_mem->q_sock.read(data, q_mem->q_length - data.size() - 
							   sizeof(PacketType) -
							   sizeof(uint8_t)) < 0) {
				return;
			}
//			cout<<"aa: " <<q_mem->q_sock.available() <<endl;
//			::exit(0);
			if(data.size() >= (q_mem->q_length-sizeof(PacketType)-sizeof(uint8_t))) {
				//read out the packet type
				data.seek(0);
				
				if(!(q_mem->q_config & EndPacket)) {
					return;
				}
				
//				cout<<"ds: " <<data.size() <<endl;
//				for(int i=0; i<15; i++) {
//					cout<<(int)data.data()[i] <<" ";
//				} cout<<endl;
				
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
//		cout<<"void ODataStream::writeInit()" <<endl;
		q_mem->q_writeState = WriteHigh;
		writeHigh();
	}
	
	void writeHigh() {
//		cout<<"void ODataStream::writeHigh()" <<endl;
		
		typename vector<ODSOutQueue>::reverse_iterator i = q_mem->q_pque.rbegin();
		
//		auto i=q_mem->q_pque.rbegin();
		//we have found the first queue, check if it contains any chunks of
		//data to write
		i->lock();
		if(i->hasData()) {
			//prepare the header for the packet
			OByteArray head;
			OByteArray& data = i->q_mem->q_que.front().q_data;
			
			//setup the variable that make up the header for this packet
			PacketLength length = 0;
			uint8_t config = EndPacket;
			PacketType type = i->q_mem->q_que.front().q_type;
			head<<length ;//cout<<"hs: " <<head.size() <<" ds: " <<data.size() <<endl;
			head<<config;//cout<<"hs: " <<head.size() <<" ds: " <<data.size() <<endl;
			head<<type;//cout<<"hs: " <<head.size() <<" ds: " <<data.size() <<endl;
			
			head.seek(0);
			length = data.size() + head.size() - sizeof(PacketLength);
			head<<length;
			
//			for(int i=0; i<head.size(); i++) {
//				cout<<(int)head.data()[i] <<" ";
//			} cout<<endl;
			
//			cout<<"l: " <<length <<" c: " <<(int)config <<" t: " <<(int)type <<endl;
//			cout<<"hs: " <<head.size() <<" ds: " <<data.size() <<endl;
			
			//assume the whole header will be written...
			head.seek(0);
			int hlen = 0;
			if((hlen = q_mem->q_sock.write(head)) < 0) {
//				cout<<"could not write" <<endl;
				//the connection was closed so just unlock the mutex and return
				i->unlock();
				return;
			}
//			cout<<"hlen: " <<hlen <<endl;
			
			//write the payload
			int plen = 0;
			if((plen = q_mem->q_sock.write(data)) < 0) {
//				cout<<"could not write" <<endl;
				//the connection was closed so just unlock the mutex and return
				i->unlock();
				return;
			}
//			cout<<"plen: " <<plen <<endl;
			
			//check if we wrote all of the payload or just most of it
			if(!data.dataLeft()) {
				//we wrote all this packet pop it from the queue
				i->q_mem->q_que.pop_front();
				
				//we are done here so reset to init
				q_mem->q_writeState = WriteInit;
				i->unlock();
				return;
			}
			
			//store the remaining packet to be written when more room in
			//the network stack becomes available
			q_mem->q_writepacket = i->q_mem->q_que.front();
			i->q_mem->q_que.pop_front();
			
			q_mem->q_writeState = WriteFinish;
			
			i->unlock();
			return;
		} else {
			//else change state to write low
			i->unlock();
			q_mem->q_writeState = WriteLow;
			writeLow();
			return;
		}
		
		i->unlock();
		q_mem->q_writeState = WriteLow;
		writeLow();
	}
	
	void writeLow() {
		//start with the first packetwe looked at last after a call to write
		//the highest priority packet
		for(typename vector<ODSOutQueue>::reverse_iterator i = 
			q_mem->q_pque.rbegin()+1; i<q_mem->q_pque.rend(); i++) {
			i->lock();
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
				if(q_mem->q_sock.write(head) < 0) {
					//the connection was closed so just unlock the mutex and return
					i->unlock();
					return;
				}
				
				//write the payload
				if(q_mem->q_sock.write(data, datalen) < 0) {
					//the connection was closed so just unlock the mutex and return
					i->unlock();
					return;
				}
				
				//check if we wrote all of the payload or just most of it
				if(!data.dataLeft()) {
					//we wrote all this packet pop it from the queue
					i->q_mem->q_que.pop_front();
					
					//we are done here so reset to init
					q_mem->q_writeState = WriteInit;
					
					i->unlock();
					return;
				}
				
				//store the remaining packet to be written when more room in
				//the network stack becomes available
				q_mem->q_writepacket = i->q_mem->q_que.front();
				i->q_mem->q_que.pop_front();
				
				q_mem->q_writeState = WriteFinish;
				
				i->unlock();
				return;
			}
			i->unlock();
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
		q_mem->q_activity.readAll();
	}
	
	void sockDisconnected() {
		//reset the writing state
		q_mem->q_writeState = WriteInit;
		
		//clear all the buffers
		q_mem->q_readhead = true;
		q_mem->q_head.clear();
		for(typename vector<ODSOutQueue>::iterator i = 
			q_mem->q_pque.begin(); i<q_mem->q_pque.end(); i++) {
			i->lock();
			i->q_mem->q_que.clear();
			i->unlock();
		}
		for(typename vector<OByteArray>::iterator i = 
			q_mem->q_recvBuff.begin(); i<q_mem->q_recvBuff.end(); i++) {
			i->clear();
		}
		
		if(q_mem->q_disconnect)
			q_mem->q_disconnect();
	}
	
	shared_ptr<ODataStreamMem>			q_mem;
};

#endif // ODATASTREAM_HPP

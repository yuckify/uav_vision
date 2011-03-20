/*	Copyright (c) 2010 Michael F. Varga
 *	Email:		michael@engineerutopia.com
 *	Website:	engineerutopia.com
 *	
 *	Permission is hereby granted, free of charge, to any person obtaining a copy
 *	of this software and associated documentation files (the "Software"), to deal
 *	in the Software without restriction, including without limitation the rights
 *	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *	copies of the Software, and to permit persons to whom the Software is
 *	furnished to do so, subject to the following conditions:
 *	
 *	The above copyright notice and this permission notice shall be included in
 *	all copies or substantial portions of the Software.
 *	
 *	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 *	THE SOFTWARE.
*/

#ifndef OO_H
#define OO_H

#if defined(__APPLE__)
	#define __apple__
#endif

#if defined(_WIN32) || defined(_WIN64)
	#define __windows__
#endif


#include<fcntl.h>
#include<ostream>

#ifdef __windows__
	#include<WinSock2.h>
	#include<WinBase.h>
	#include<Windows.h>
	#include<time.h>
//	#include<WS2tcpip.h> unused
//	#include<io.h> unused
//	#include<dos.h> unused

	
	typedef INT64 int64_t;
	typedef UINT64 uint64_t;

	#define bzero(a, b) memset(a, 0, b)

#else
//	#define SOCKET int
//	#define HANDLE int
	#include<netdb.h>
	#include<termios.h>
#endif

//include all components if OO_ALL is defined
#ifdef OO_ALL
#define OO_OPENCV

#endif

using namespace std;

/**	\mainpage
 *	The socket classes provide an easy to use object oriented interface. The
 *	socket and serial classes are ment to be registered with the GThread class. 
 *	GThread handles in the background all the issues of synchronizing I/O 
 *	determining when they can handle more data, when data is available and
 *	when errors occur. GThread also manages a delta list for use with the
 *	GTimer class.
*/

/**	GS is the main namespace that contains globalally used configuration
 *	parameters.
*/
namespace OO {
#ifdef __windows__
	typedef HANDLE HANDLE;
	typedef SOCKET SOCKET;
#else
	typedef int HANDLE;
	typedef int SOCKET;
#endif
	
	enum ArrayBase {
		Input,
		Output
	};
	
	enum IOBase {
		end,
		beg,
		cur
	};
	
	/**	\brief Logic to be used with a container filter function.
	 *	
	 *	These flags specify the logic to perform when using one of the filter
	 *	functions with the the container classes GAddressList or GNetList.
	 *	See the specific containers for specific used on these flags.
	 *	@see GNetList
	 *	@see GAddressInfo
	*/
	enum MatchType {
		And,
		Not
	};
	
	/**	\brief Type of a socket.
	 *	
	 *	
	 *	
	*/
	enum SockType {
		NullType = 0,
		Udp = SOCK_DGRAM,
		Raw = SOCK_RAW,
		Tcp = SOCK_STREAM
	};
	
	/**	\brief GSockFamily flags are used with the creation of GAddressInfo or GSockAddress
	 *	to open a connection or to limit the output of GSocket::getAddressInfo().
	*/
	enum SockFamily {
		///	Internet Protocol version 4.
		Inet4 = AF_INET,
		
		/// Internet Protocol version 6.
		Inet6 = AF_INET6,
		
		/// Unix domain socket, typically used for interprocess communications.
		Unix = AF_UNIX,
		
		///	Specify that any family ( IPv4 or IPv6 ) may be used with the creation
		/// of a connection or connectionless socket.
		NullFamily = AF_UNSPEC
	};
	
	/**	\brief GSockFlags are used with a call to GSocket::getAddressInfo()
	 *	to narrow the search results or input parameters.
	*/
	enum SockFlags {
		NullFlag = 0,
		
		/**	If this flag is passed the returned address may be used in a call to bind 
		 *	for the specified service to listen fo incomming connections. If this
		 *	flag is not used the returned address is suitable for a call to 
		 *	GTcpSocket::connect() or GUdpSocket::connect().
		*/
		Passive = AI_PASSIVE,
		
		/**	With this flag a numeric host string address shall be supplied
		 *	for the 'address' parameter which prevents a DNS resolution.
		*/
		NumericHost = AI_NUMERICHOST,
		
		/**	If this flag is used and the 'address' parameter is not an empty string
		 *	then the function will attempt to determine a canonical name as if 
		 *	'address' were a shorthand name.
		*/
		CanonicalName = AI_CANONNAME,
		
		/**	If this flag is used the 'service' parameter shall be a numeric port 
		 *	string. This shall prevent any name resolution service (NIS+) from being
		 *	invoked.
		*/
		NumericServ = AI_NUMERICSERV,
		
		/**	
		 *	
		 *	
		*/
		V4Mapped = AI_V4MAPPED,
		
		/**	
		 *	
		 *	
		*/
		All = AI_ALL,
		
		/**	If this flag is used then an either an IPv4 or IPv6 may be returned
		 *	if it has been configure on the local system.
		*/
		AddrConfig = AI_ADDRCONFIG
	};
	
	enum SockProto {
		ProtoIpv4 = IPPROTO_IP,
		ProtoIpv6 = IPPROTO_IPV6,
		ProtoIcmp = IPPROTO_ICMP,
		ProtoRaw = IPPROTO_RAW,
		ProtoTcp = IPPROTO_TCP,
		ProtoUdp = IPPROTO_UDP
	};
	
	enum SockScope {
		
	};
	
	enum SockFlow {
		
	};
	
	enum TimerType {
		Once,
		Repeat
	};
	
	/**	\brief Specify the type of the serial port.
	 *	
	*/
	enum SerialType {
		/// This flag specifies that a GPort instance is empty.
		NullPort,
		
		/// This flag specifies that a GPort instance is referenceing a
		/// usb to serial adapter.
		UsbPort,
		
		/// This flag specifies that a GPort instance is reference a 
		/// harware serial port.
		HwPort
	};
	
	/**	\brief Options to open a serial port with.
	*/
	enum SerialOpt {
		
		/**	If this is set a call to open() for reading only shall return
		 *	without delay. An open() for writing only shall return an error
		 *	if no process currently has the file open for reading. If this is
		 *	not set a call to open() for reading only shall block until
		 *	a thread opens the file for writing. A call to open() for writing
		 *	only shall block until a thread opens the file for reading.
		*/
#ifndef __windows__
		NonBlock = O_NONBLOCK,
#endif
		
		/**	If the file is opened successfuly with write permissions, its
		 *	length shall be truncated to 0.
		*/
		Truncate = O_TRUNC,
		
		/// If the file exists nothing happens, else the file shall be created.
		Create = O_CREAT,
		
		/// If set, the file offset shall be set to the end of the file prior to
		/// each write.
		Appen = O_APPEND,
		
		///	Open the terminal for reading only.
		Read = O_RDONLY,
		
		/// Open the terminal for writing only.
		Write = O_WRONLY,
		
		/// Open the terminal for reading and writing.
		ReadWrite = O_RDWR,
		
		/// If the port is a teminal it will not become the controlling terminal
		/// for the program.
#ifdef __windows__
		NoControlTTY = 0,
#else
		NoControlTTY = O_NOCTTY,
#endif
		
		/** \brief Default options when none are specified.
		 *	
		 *	By default the port is opened with read/write permissions
		 *	and the port does not act as the controlling terminal for
		 *	the program if it is a terminal.
		 */
#ifdef __windows__
		DefaultOpts = 0,
#else
		DefaultOpts = ReadWrite | NoControlTTY,
#endif
	};
	
	enum SerialSpeed {
#ifdef __windows__
//		O0			= CBR_0,
//		O50			= CBR_50,
//		O75			= CBR_75,
		O110		= CBR_110,
//		O134		= CBR_134,
//		O150		= CBR_150,
//		O200		= CBR_200,
		O300		= CBR_300,
		O600		= CBR_600,
		O1200		= CBR_1200,
//		O1800		= CBR_1800,
		O2400		= CBR_2400,
		O4800		= CBR_4800,
		O9600		= CBR_9600,
		O19200		= CBR_19200,
		O38400		= CBR_38400,
		O57600		= CBR_57600,
		O115200		= CBR_115200,
//		O230400		= CBR_230400,
#else
//		O0			= B0,
//		O50			= B50,
//		O75			= B75,
		O110		= B110,
//		O134		= B134,
//		O150		= B150,
//		O200		= B200,
		O300		= B300,
		O600		= B600,
		O1200		= B1200,
//		O1800		= B1800,
		O2400		= B2400,
		O4800		= B4800,
		O9600		= B9600,
		O19200		= B19200,
		O38400		= B38400,
		O57600		= B57600,
		O115200		= B115200,
//		O230400		= B230400,
/*
#ifdef __linux__
		O460800		= B460800,
		O500000		= B500000,
		O576000		= B576000,
		O921600		= B921600,
		O1000000	= B1000000,
		O1152000	= B1152000,
		O1500000	= B1500000,
		O2000000	= B2000000,
		O2500000	= B2500000,
		O3000000	= B3000000,
		O3500000	= B3500000,
		O4000000	= B4000000
#endif
*/
#endif
	};
	
};

ostream& operator<<(ostream& out, OO::SockType t);
ostream& operator<<(ostream& out, OO::SockFamily f);
ostream& operator<<(ostream& out, OO::SockFlags f);
ostream& operator<<(ostream& out, OO::SockProto p);

#endif // OO_H

#include"OSockError.hpp"

OSockError::OSockError(int e) {
	se_code = e;
}

int OSockError::code() const {
	return se_code;
}

OString OSockError::string() const {
	switch(se_code) {
		//accept()
		case EAGAIN:		return "O_NONBLOCK is set for the socket file "
			"descriptor and no connections are present to be accepted.";
		case EBADF:			return "The socket argument is not a valid file "
					"descriptor.";
		case ECONNABORTED:	return "A connection has been aborted.";
		case EINTR:			return "The accept() function was interrupted by "
					"a signal that was caught before a valid connection arrived.";
		case EINVAL:		return "The socket is not accepting connections.";
		case EMFILE:		return "{OPEN_MAX} file descriptors are currently "
					"open in the calling process.";
		case ENFILE:		return "The maximum number of file descriptors in "
					"the system are already open.";
		case ENOTSOCK:		return "The socket argument does not refer to a socket.";
		case EOPNOTSUPP:	return "The socket type of the specified socket "
					"does not support accepting connections.";
		case ENOBUFS:		return "No buffer space is available.";
		case ENOMEM:		return "There was insufficient memory available to "
					"complete the operation.";
		case EPROTO:		return "A protocol error has occurred; for example, "
					"the STREAMS protocol stack has not been initialized.";
		
		//bind()
		case EADDRINUSE:	return "The specified address is already in use.";
		case EADDRNOTAVAIL:	return "The specified address is not available "
					"from the local machine.";
		case EAFNOSUPPORT:	return "The specified address is not a valid "
					"address for the address family of the specified socket.";
		case EACCES:		return "A component of the path prefix denies "
					"search permission, or the requested name requires writing "
					"in a directory with a mode that denies write permission.";
		case EISDIR:		return "The address argument is a null pointer.";
		case EIO:			return "An I/O error occurred.";
		case ELOOP:			return "A loop exists in symbolic links encountered "
					"during resolution of the pathname in address.";
		case ENAMETOOLONG:	return "A component of a pathname exceeded "
			"{NAME_MAX} characters, or an entire pathname exceeded {PATH_MAX} "
			"characters.";
		case ENOENT:		return "A component of the pathname does not name "
					"an existing file or the pathname is an empty string.";
		case ENOTDIR:		return "A component of the path prefix of the "
					"pathname in address is not a directory.";
		case EROFS:			return "The name would reside on a read-only file "
					"system.";
		case EISCONN:		return "The socket is already connected.";
		
		//connect()
		case EALREADY:		return "A connection request is already in progress "
					"for the specified socket.";
		case ECONNREFUSED:	return "The target address was not listening for "
					"connections or refused the connection request.";
		case EINPROGRESS:	return "O_NONBLOCK is set for the file descriptor "
					"for the socket and the connection cannot be immediately "
					"established; the connection shall be established "
					"asynchronously.";
		case ENETUNREACH:	return "No route to the network is present.";
		case EPROTOTYPE:	return "The specified address has a different type "
					"than the socket bound to the specified peer address.";
		case ETIMEDOUT:		return "The attempt to connect timed out before a "
					"connection was made.";
		case ECONNRESET:	return "Remote host reset the connection request.";
		case EHOSTUNREACH:	return "The destination host cannot be reached "
					"(probably because the host is down or a remote router "
					"cannot reach it).";
		case ENETDOWN:		return "The local network interface used to reach "
					"the destination is down.";
		
		//listen()
		case EDESTADDRREQ:	return "The socket is not bound to a local address, "
					"and the protocol does not support listening on an unbound "
					"socket.";
		
		//recv()
		case ENOTCONN:		return "A receive is attempted on a connection-mode "
					"socket that is not connected.";
		
		//recvfrom()
		
		//send()
		case EMSGSIZE:		return "The message is too large to be sent all at "
					"once, as the socket requires.";
		case EPIPE:			return "The socket is shut down for writing, or the "
					"socket is connection-mode and is no longer connected. In "
					"the latter case, and if the socket is of type SOCK_STREAM, "
					"the SIGPIPE signal is generated to the calling thread.";
		
		//sendto()
		
		//setsockopt()
		case EDOM:			return "The send and receive timeout values are too big to fit into the timeout fields in the socket structure.";
		case ENOPROTOOPT:	return "The option is not supported by the protocol.";
		
		//getsockopt()
		
	}
	return "Undefined Error";
}

ostream& operator<<(ostream& out, OSockError e) {
	out<<e.string();
	return out;
}

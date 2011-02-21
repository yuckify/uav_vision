#include"OPipeError.hpp"

OPipeError::OPipeError(int e) {
	pe_code = e;
}

int OPipeError::code() const {
	return pe_code;
}

OString OPipeError::string() const {
	switch(pe_code) {
		//write()
	case EAGAIN: return "The O_NONBLOCK flag is set for the "
				"file descriptor and the thread would be delayed "
				"in the write() operation.";
	case EBADF: return "The fildes argument is not a valid file "
				"descriptor open for writing.";
	case EFBIG: return "An attempt was made to write a file that "
				"exceeds the implementation-defined maximum file "
				"size or the process' file size limit, and there "
				"was no room for any bytes to be written.";
	case EINTR: return "The write operation was terminated due "
				"to the receipt of a signal, and no data was "
				"transferred.";
	case EIO: return "The process is a member of a background "
				"process group attempting to write to its "
				"controlling terminal, TOSTOP is set, the process "
				"is neither ignoring nor blocking SIGTTOU, and "
				"the process group of the process is orphaned. "
				"This error may also be returned under "
				"implementation-defined conditions.";
	case ENOSPC: return "There was no free space remaining on "
				"the device containing the file.";
	case EPIPE: return "An attempt is made to write to a pipe or "
				"FIFO that is not open for reading by any process, "
				"or that only has one end open. A SIGPIPE signal "
				"shall also be sent to the thread.";
	case ERANGE: return "The transfer request size was outside "
				"the range supported by the STREAMS file associated "
				"with fildes.";
	case EINVAL: return "The STREAM or multiplexer referenced by "
				"fildes is linked (directly or indirectly) "
				"downstream from a multiplexer.";
	case ENOBUFS: return "Insufficient resources were available "
				"in the system to perform the operation.";
	case ENXIO:	return "A request was made of a nonexistent device, "
				"or the request was outside the capabilities of the "
				"device.";

		//read()
	case EBADMSG: return "The file is a STREAM file that is set to "
				"control-normal mode and the message waiting to be "
				"read includes a control part. ";
	case EISDIR: return "The fildes argument refers to a directory "
				"and the implementation does not allow the "
				"directory to be read using read(), pread() or "
				"readv(). The readdir() function should be used "
				"instead. ";
	case EOVERFLOW:	return "The file is a regular file, nbyte is "
				"greater than 0, the starting position is before "
				"the end-of-file and the starting position is "
				"greater than or equal to the offset maximum "
				"established in the open file description "
				"associated with fildes. ";

		//pipe()
	case EMFILE: return "More than {OPEN_MAX} minus two file "
				"descriptors are already in use by this process.";
	case ENFILE: return "The number of simultaneously open files "
				"in the system would exceed a system-imposed limit.";
	}
	
	//close()
	
	return "Undefined Error";
}


ostream& operator<<(ostream& out, OPipeError e) {
	out<<e.string();
	return out;
}

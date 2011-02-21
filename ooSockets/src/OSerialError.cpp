#include"OSerialError.hpp"

OSerialError::OSerialError(int e) {
	se_code = e;
}

int OSerialError::code() const {
	return se_code;
}

OString OSerialError::string() const {
	switch(se_code) {
	//open()
	case EACCES:		return "Search permission is denied on a component of"
				"the path prefix, or the file exists and the permissions specified "
				"by oflag are denied, or the file does not exist and write "
				"permission is denied for the parent directory of the file to be "
				"created, or O_TRUNC is specified and write permission is denied.";
	case EEXIST:		return "O_CREAT and O_EXCL are set, and the named file exists.";
	case EINTR:			return "A signal was caught during open().";
	case EINVAL:		return "The implementation does not support synchronized "
				"I/O for this file.";
	case EIO:			return "The path argument names a STREAMS file and a hangup "
				"or error occurred during the open().";
	case EISDIR:		return "The named file is a directory and oflag includes "
				"O_WRONLY or O_RDWR.";
	case ELOOP:			return "A loop exists in symbolic links encountered during "
				"resolution of the path argument.";
	case EMFILE:		return "{OPEN_MAX} file descriptors are currently open in "
				"the calling process.";
	case ENAMETOOLONG:	return "The length of the path argument exceeds {PATH_MAX} "
				"or a pathname component is longer than {NAME_MAX}.";
	case ENFILE:		return "The maximum allowable number of files is currently "
				"open in the system.";
	case ENOENT:		return "O_CREAT is not set and the named file does not exist; "
				"or O_CREAT is set and either the path prefix does not exist or the "
				"path argument points to an empty string.";
	case ENOSR:			return "The path argument names a STREAMS-based file and the "
				"system is unable to allocate a STREAM. [Option End]";
	case ENOSPC:		return "The directory or file system that would contain the "
				"new file cannot be expanded, the file does not exist, and O_CREAT is "
				"specified.";
	case ENOTDIR:		return "A component of the path prefix is not a directory.";
	case ENXIO:			return "O_NONBLOCK is set, the named file is a FIFO, O_WRONLY "
				"is set, and no process has the file open for reading.";
	case EOVERFLOW:		return "The named file is a regular file and the size of the "
				"file cannot be represented correctly in an object of type off_t.";
	case EROFS:			return "The named file resides on a read-only file system and "
				"either O_WRONLY, O_RDWR, O_CREAT (if the file does not exist), or "
				"O_TRUNC is set in the oflag argument.";
	case EAGAIN:		return "The path argument names the slave side of a "
				"pseudo-terminal device that is locked. [Option End]";
	case ENOMEM:		return "The path argument names a STREAMS file and the system "
				"is unable to allocate resources.";
	case ETXTBSY:		return "The file is a pure procedure (shared text) file that "
				"is being executed and oflag is O_WRONLY or O_RDWR.";
	
	//tcgetattr()
	case EBADF:			return "The fildes argument is not a valid file descriptor.";
	case ENOTTY:		return "        The file associated with fildes is not a terminal.";
	
	//tcsetattr()
	
	default:			return "Undefined Error";
	}
}

ostream& operator<<(ostream& out, OSerialError e) {
	out<<e.string();
	return out;
}

#ifndef OTERMINAL_H
#define OTERMINAL_H

#include<functional>
#include<map>
#include<stdio.h>
#include<stropts.h>
#include<iostream>
#include<sys/ioctl.h>
#include<vector>

#include<OString.hpp>
#include<OThread.hpp>
#include<OIODevice.hpp>

using namespace std;

class OTerminal;

class OTerminalArg {
	friend class OTerminal;
public:
	
	OString string() {
		return str;
	}
	
	OString operator[](int i) {
		return split[i];
	}
	
	unsigned size() const {
		return split.size();
	}
	
protected:
	OStringList split;
	OString str;
};

/**	This class acts as a means to easily create an 
 *	interactive terminal. The source is selectable
 *	as either stdin or user defined source. If a 
 *	user defined source is selected the instance
 *	does not need to be registered with a OThread
 *	instance.
*/
class OTerminal : public OIODevice {
public:
	enum Source {
		Stdin,
		Manual
	};
	
	/**	Create an interactive terminal instance that takes its
	 *	input from a user defined source.
	*/
	OTerminal();
	
	/**	Create an interactive terminal instance that takes its
	 *	input from stdin. If stdin is selected as the source
	 *	this instance must be registered with and OThread instance.
	*/
	OTerminal(OThread* parent, Source source = Stdin);
	
	/**	If stdin is selected as the input the file descriptor
	 *	for stdin is returned otherwise -1.
	*/
	OO::HANDLE handle() const;
	
	/**	If stdin is selected as the input the amount of data
	 *	available on stdin is returned otherwise 0.
	*/
	int available();
	
	void readLoop();
	void writeLoop();
	void priorityLoop();
	
	void setParent(OThread* parent);
	OThread* parent() const;
	
	OString delim();
	void setDelim(OString str);
	
	/**	This function process the input string, one line at a time.
	 *	If user input is selected the input string must be passed
	 *	to this function.
	*/
	void handleInput(OString input);
	
	/**	Register a function that is called when the terminal receives
	 *	the desired command.
	 *	@param cmd The command to look for.
	 *	@param cbk The function to call when the desired command
	 *	is received.
	*/
	void registerCase(OString cmd, function<void (OTerminalArg)> cbk);
	
	/**	When the string does not represent any known command 
	 *	this function is called.
	*/
	void registerDefault(function<void (OTerminalArg)> cbk);
	
protected:
	
	typedef vector< pair<OString, function<void (OTerminalArg)> > > FMapType;
	
	FMapType fmap;
	function<void (OTerminalArg)> cdef;
	OThread* par;
	Source src;
	OString d;
};

#endif // OTERMINAL_H

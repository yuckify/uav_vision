#include "OTerminal.h"

OTerminal::OTerminal() {
	par = NULL;
	src = Manual;
	d = " ";
}

OTerminal::OTerminal(OThread *parent, Source source) {
	par = parent;
	src = source;
	d = " ";
	
	if(par && src == Stdin) par->registerReadFD(STDIN_FILENO, this);
}

OO::HANDLE OTerminal::handle() const {
	if(src == Stdin)
		return STDIN_FILENO;
	
	return -1;
}

int OTerminal::available() {
	if(src == Stdin) {
		int sum = 0;
		
		ioctl(STDIN_FILENO, FIONREAD, &sum);
		
		return sum;
	}
	
	return 0;
}

void OTerminal::readLoop() {
	//get some text from stdin
	OString input;
	cin>>input;
	
	//process the text
	handleInput(input);
}

void OTerminal::writeLoop() {
	
}

void OTerminal::priorityLoop() {
	
}

void OTerminal::setParent(OThread* parent) {
	if(par) par->unregisterReadFD(STDIN_FILENO);
	
	par = parent;
	
	par->registerReadFD(STDIN_FILENO, this);
}

OThread* OTerminal::parent() const {
	return par;
}

OString OTerminal::delim() {
	return d;
}

void OTerminal::setDelim(OString str) {
	d = str;
}

void OTerminal::handleInput(OString input) {
	OTerminalArg arg;
	arg.str = input;
	arg.split = input.split(d);
	
	if(input.size() >= 1) {
		for(unsigned i=0; i<fmap.size(); i++) {
			//if we found the command-function map
			//prepare the string and call the function
			if(input.startsWith(fmap[i].first)) {
				int cmdlen = fmap[i].first.length();
				OString a = input.substring(cmdlen);
				
				//if there is a delimiter at the beginning of
				//the input string trim it until there are none left
				for(unsigned j=0; j<a.length(); j++) {
					if(a.startsWith(d)) {
						a = a.substring(d.length());
					} else
						break;
				}
				
				arg.str = a;
				arg.split = a.split(d);
				
				//call the function
				fmap[i].second(arg);
				
				//we are done here
				return;
			}
		}
		
		//fallthrough, call the default function
		if(cdef) cdef(arg);
	}
}

void OTerminal::registerCase(OString cmd, function<void (OTerminalArg)> cbk) {
	fmap.push_back(pair<OString, function<void (OTerminalArg)> >(cmd, cbk));
}

void OTerminal::registerDefault(function<void (OTerminalArg)> cbk) {
	cdef = cbk;
}

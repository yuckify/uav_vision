#include"OTimer.hpp"

OTimer::OTimer(OThread *parent) {
	par = parent;
}

OTimer::~OTimer() {
	unregisterTimer();
}

OThread* OTimer::parent() {
	return par;
}

void OTimer::parent(OThread *p) {
	unregisterTimer();
	par = p;
	registerTimer();
}

void OTimer::start(int ms, OO::TimerType t) {
	unregisterTimer();
	tim.msec(ms);
	ty = t;
	registerTimer();
}

void OTimer::stop() {
	unregisterTimer();
}

OTime OTimer::period() {
	return tim;
}

function<void ()> OTimer::callback() {
	return cbk;
}

void OTimer::runLoop() {
	if(cbk)
		cbk();
}

OO::TimerType OTimer::type() {
	return ty;
}

void OTimer::callback(function<void ()> cbk) {
	this->cbk = cbk;
}

void OTimer::registerTimer() {
	if(par)
		par->registerTimer(this);
}

void OTimer::unregisterTimer() {
	if(par)
		par->unregisterTimer(this);
}

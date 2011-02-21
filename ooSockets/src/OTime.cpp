#include"OTime.hpp"

OTime::OTime() {
	us = 0;
}

OTime::OTime(int msec) {
	us = msec * 1000;
}

OTime::OTime(struct timeval t) {
	us = t.tv_sec + t.tv_usec * 1000000;
}

void OTime::setCurrent() {
	struct timeval tim;
	gettimeofday(&tim, 0);
	us = tim.tv_sec * 1000000 + tim.tv_usec;
}

int OTime::elapsed() const {
	OTime elap(current().operator-(*this));
	return elap.sec()*1000 + elap.msec();
}

uint64_t OTime::uelapsed() const {
	OTime elap(current().operator-(*this));
	return elap.sec()*1000000 + elap.usec();
}

OTime OTime::operator-(OTime t) {
	OTime tmp;
	tmp.us = us - t.us;
	return tmp;
}

OTime OTime::operator+(OTime t) {
	OTime tmp;
	tmp.us = us + t.us;
	return tmp;
}

OTime& OTime::operator-=(const OTime& other) {
	us -= other.us;
	return *this;
}

OTime& OTime::operator+=(const OTime& other) {
	us += other.us;
	return *this;
}

OTime OTime::current() {
	OTime tmp;
	tmp.setCurrent();
	return tmp;
}

timeval OTime::toTimeval() {
	timeval tmp;
	tmp.tv_sec = us/1000000;
	tmp.tv_usec = us%1000000;
	return tmp;
}

int OTime::sec() const {
	return us/1000000;
}

int OTime::msec() const {
	return usec()/1000;
}

int64_t OTime::usec() const {
	return us;
}

void OTime::sec(int t) {
	us = t * 1000000;
}

void OTime::msec(int t) {
	us = t * 1000;
}

void OTime::usec(int64_t t) {
	us = t;
}

int OTime::compare(const OTime& t) const {
	//if t is greater then return -1
	if(usec() < t.usec())
		return -1;
	//if t is less than return 1
	else if(usec() > t.usec())
		return 1;
	//else the two OTimes must be equal so return 0
	return 0;
}

bool OTime::operator<(const OTime& t) const {
	return compare(t) < 0 ? true : false;
}

bool OTime::operator<=(const OTime& t) const {
	return compare(t) <= 0 ? true : false;
}

bool OTime::operator>(const OTime& t) const {
	return compare(t) > 0 ? true : false;
}

bool OTime::operator>=(const OTime& t) const {
	return compare(t) >= 0 ? true : false;
}

bool OTime::operator==(const OTime& t) const {
	return compare(t) == 0 ? true : false;
}

ostream& operator<<(ostream& out, const OTime& t) {
	out<<t.msec();
	return out;
}

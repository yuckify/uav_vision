#include "OVersion.hpp"

OVersion::OVersion(OString str) {
	set(str);
}

void OVersion::operator=(OString str) {
	set(str);
}

void OVersion::set(OString str) {
	OStringList args = str.split(".");
	if(args.size() > 0) {
		bool ok = false;
		a = args[0].toUInt(&ok);
		if(!ok) a = 0;
		
		if(args.size() > 1) {
			b = args[1].toUInt(&ok);
			if(!ok) b = 0;
			
			if(args.size() > 2) {
				c = args[2].toUInt(&ok);
				if(!ok) c = 0;
			} else {
				c = 0;
			}
		} else {
			b = 0;
			c = 0;
		}
	} else {
		a = 0;
		b = 0;
		c = 0;
	}
}

bool OVersion::operator>(const OVersion& ver) const {
	return compare(ver) > 0;
}

bool OVersion::operator>=(const OVersion& ver) const {
	return compare(ver) >= 0;
}

bool OVersion::operator<(const OVersion& ver) const {
	return compare(ver) < 0;
}

bool OVersion::operator<=(const OVersion& ver) const {
	return compare(ver) <= 0;
}

bool OVersion::operator==(const OVersion& ver) const {
	return compare(ver) == 0;
}

int OVersion::compare(const OVersion &ver) const {
	//all equal
	if(a == ver.a && b == ver.b && c == ver.c) return 0;
	
	//a has the highest precedence b second and c third
	//compare the OVersion numbers in order of precedence
	if(a > ver.a) {
		return 1;
	} else {
		if(b > ver.b) {
			return 1;
		} else {
			if(c > ver.c) {
				return 1;
			} else {
				return -1;
			}
		}
	}
	return -1;
}

unsigned OVersion::major() {
	return a;
}

unsigned OVersion::mid() {
	return b;
}

unsigned OVersion::minor() {
	return c;
}

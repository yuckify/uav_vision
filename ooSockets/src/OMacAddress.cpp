#include"OMacAddress.hpp"

OMacAddress::OMacAddress() {
	::memset(m, 0, _GMAC_SIZE);
}

OMacAddress::OMacAddress(const void *dat) {
	::memcpy(m, dat, _GMAC_SIZE);
}

OMacAddress::OMacAddress(OString s) {
	if(!str(s)) {
		::memset(m, 0, _GMAC_SIZE);
	}
}

OString OMacAddress::str() const {
	ostringstream str;
	str<<hex;
	for(int i=0; i<_GMAC_SIZE; i++) {
		unsigned tmp = m[i];
		str<<(((unsigned)tmp) & 0x000000ff);
		if(i != 5) str<<":";
	}
	return str.str();
}

bool OMacAddress::str(OString s) {
	OStringList ops;
	
	if(s.contains(":")) {
		ops = s.split(":");
	} else if(s.contains("-")) {
		ops = s.split("-");
	} else {
		return false;
	}
	
	if(ops.size() == _GMAC_SIZE) {
		for(int i=0; i<_GMAC_SIZE; i++) {
			bool ok;
			m[i] = ops[i].toInt(&ok, 16);
			if(!ok) {
				return false;
			}
		}
	} else {
		return false;
	}
	return true;
}

char* OMacAddress::data() {
	return m;
}

const char* OMacAddress::constData() const {
	return m;
}

unsigned OMacAddress::size() const {
	return _GMAC_SIZE;
}

bool OMacAddress::isEmpty() const {
	return m[0] + m[1] + m[2] + m[3] + m[4] + m[5];
}

bool OMacAddress::operator==(OMacAddress&& other) {
	return ::memcmp(m, other.m, _GMAC_SIZE) == 0;
}

OByteArray& operator<<(OByteArray& data, OMacAddress& addr) {
	data.append(addr.data(), addr.size());
	return data;
}

OByteArray& operator>>(OByteArray& data, OMacAddress& addr) {
	data.read(addr.data(), addr.size());
	return data;
}

#include"OString.hpp"

OString::OString() : string() {
	
}

OString::OString(string str) : string(str) {
	
}

OString::OString(const char* str) : string(str) {
	
}

OString::OString(const char* str, int len) : string(str, len) {
	
}

OString::OString(const OString& str) : string(str.c_str()) {
	
}

#if defined(_O_QSTRING_SUPPORT_)
OString::OString(QString str) {
	int length = str.length();
	for(int i=0; i<length; i++) {
		push_back(str[i].toAscii());
	}
}
#endif

OString& OString::operator=(short i) {
	*this = number(i);
	return *this;
}

OString& OString::operator=(unsigned short i) {
	*this = number(i);
	return *this;
}

OString& OString::operator=(int i) {
	*this = number(i);
	return *this;
}

OString& OString::operator=(unsigned int i) {
	*this = number(i);
	return *this;
}

OString& OString::operator=(long i) {
	*this = number(i);
	return *this;
}

OString& OString::operator=(unsigned long i) {
	*this = number(i);
	return *this;
}

OString& OString::operator=(double i) {
	*this = number(i);
	return *this;
}

OString& OString::operator=(float i) {
	*this = number(i);
	return *this;
}

OString& OString::operator<<(OString str) {
	string::append(str);
	return *this;
}

OString& OString::operator<<(string& str) {
	string::append(str);
	return *this;
}

OString& OString::operator<<(const char* str) {
	string::append(str);
	return *this;
}

OString& OString::operator<<(const char ch) {
	string::push_back(ch);
	return *this;
}

OString& OString::operator<<(bool i) {
	append(i);
	return *this;
}

OString& OString::operator<<(short i) {
	append(i);
	return *this;
}

OString& OString::operator<<(unsigned short i) {
	append(i);
	return *this;
}

OString& OString::operator<<(int i) {
	append(i);
	return *this;
}

OString& OString::operator<<(unsigned int i) {
	append(i);
	return *this;
}

OString& OString::operator<<(long i) {
	append(i);
	return *this;
}

OString& OString::operator<<(unsigned long i) {
	append(i);
	return *this;
}

OString& OString::operator<<(float i) {
	append(i);
	return *this;
}

OString& OString::operator<<(double i) {
	append(i);
	return *this;
}

OString& OString::toLower() {
	int length = this->length();
	for(int i=0; i<length; i++) {
		register char ch = at(i);
		if(ch >= 'A' && ch <= 'Z') {
			at(i) = ch - 'A' + 'a';
		}
	}
	return *this;
}

OString& OString::toUpper() {
	int len = length();
	for(int i=0; i<len; i++) {
		register char ch = at(i);
		if(ch >= 'a' && ch <= 'z') {
			at(i) = ch - 'a' + 'A';
		}
	}
	return *this;
}

uint64_t OString::hashSDBM() const {
	register unsigned long hash = 0;
	register int ch = 0;
	register const char * strptr = this->toCString();
	
	while((ch = *strptr++)) {
		hash = ch + (hash << 6) + (hash << 16) - hash;
	}
	
	return hash;
}

uint64_t OString::hashAP() const {
	register const char* strptr = this->toCString();
	register unsigned long hash = 0;
	
	while(*strptr) {
		if((unsigned long)strptr & 1) {
			hash ^= (hash << 7) ^ (*strptr) ^ (hash >> 3);
		} else {
			hash ^= ~((hash << 11) ^ (*strptr) ^ (hash >> 5));
		}
		
		strptr++;
	}
	
	return hash;
}

uint64_t OString::hashRS() const {
	register int a = 63689;
	register int b = 378551;
	register unsigned long hash = 0;
	register const char* strptr = this->toCString();
	
	while(*strptr) {
		hash = hash*a + *strptr;
		a *= b;
		strptr++;
	}
	
	
	return hash;
}

uint64_t OString::hash() const {
	return hashSDBM();
}

#if defined(_G_QOString_SUPPORT_)
QOString OString::toQOString() const {
	QOString output;
	int len = length();
	for(int i=0; i<len; i++) {
		output.push_back(at(i));
	}
	return output;
}
#endif

bool OString::isEmpty() const {
	return empty();
}

void OString::pop_back() {
	if(size() > 1)
		erase(size() - 1, 1);
}

void OString::pop_front() {
	if(size() > 1)
		erase(0, 1);
}

void OString::appendFront(const string &str) {
	insert(0, str);
}

void OString::appendFront(const char ch) {
	insert(0, 1, ch);
}

//ashley was hereizzle!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

void OString::append(const OString &str) {
	string::append(str);
}

void OString::append(bool i) {
	if(i) string::push_back('1');
	else string::push_back('0');
}

void OString::append(short i) {
	append(number(i));
}

void OString::append(unsigned short i) {
	append(number(i));
}

void OString::append(int i) {
	append(number(i));
}

void OString::append(unsigned int i) {
	append(number(i));
}

void OString::append(long i) {
	append(number(i));
}

void OString::append(unsigned long i) {
	append(number(i));
}

void OString::append(float i) {
	append(number(i));
}

void OString::append(double i) {
	append(number(i));
}

const char* OString::toCString() const {
	return c_str();
}

OString OString::operator+(const OString str) {
	string out(*((string*)this));
	out.append(str);
	return out;
}

OString OString::operator+(const char* str) {
	string out(*((string*)this));
	out.append(str);
	return out;
}

OString OString::operator+(const string& str) {
	string out(*((string*)this));
	out.append(str);
	return out;
}

OString OString::operator+(const char c) {
	string out(*((string*)this));
	out.push_back(c);
	return out;
}

OString OString::operator+(bool i) {
	OString out(*this);
	out.append(i);
	return out;
}

OString OString::operator+(short i) {
	OString out(*this);
	out.append(number(i));
	return out;
}

OString OString::operator+(unsigned short i) {
	OString out(*this);
	out.append(number(i));
	return out;
}

OString OString::operator+(int i) {
	OString out(*this);
	out.append(number(i));
	return out;
}

OString OString::operator+(unsigned int i) {
	OString out(*this);
	out.append(number(i));
	return out;
}

OString OString::operator+(long i) {
	OString out(*this);
	out.append(number(i));
	return out;
}

OString OString::operator+(unsigned long i) {
	OString out(*this);
	out.append(number(i));
	return out;
}

OString OString::operator+(float i) {
	OString out(*this);
	out.append(number(i));
	return out;
}

OString OString::operator+(double i) {
	OString out(*this);
	out.append(number(i));
	return out;
}

OString OString::substring(int start, int end) const {
	return substr(start, end - start);
}

OString OString::substringBefore(const OString str) const {
	OString xfer = this->substring(0, this->indexOf(str));
	return xfer;
}

OString OString::substringBefore(const char* str) const {
	OString xfer = this->substring(0, this->indexOf(str));
	return xfer;
}

OString OString::mid(int i, int len) const {
	return substring(i, len);
}

OString OString::substringAfter(const OString str) const {
	int afterloc = this->indexOf(str) + str.length();
	OString xfer = this->substring(afterloc, this->length());
	return xfer;
}

OString OString::substringAfter(const char* str) const {
	int strlen = 0;
	const char* strptr = str;
	while(*strptr++)
		strlen++;
	int afterloc = this->indexOf(str) + strlen;
	OString xfer = this->substring(afterloc, this->length());
	return xfer;
}

OList<OString> OString::split(const char* exp) const {
	OList<OString> tmp;
	
	const char* strptr = exp;
	int strlen = 0;
	while(*strptr++)
		strlen++;
	
	if(strlen == 1)
		return split((char)exp[0]);
	
	int begin = 0;
	int end = 0;
	int e = 0;
	
	while((end = indexOf(exp, begin + 1)) != -1) {
		tmp.push_back(substring(begin + e, end));
		begin = end + strlen;
		e = strlen;
	}
	tmp.push_back(substring(begin, size()));
	
	return tmp;
}

OList<OString> OString::split(const OString& exp) const {
	OList<OString> tmp;
	
	int strlen = exp.length();
	
	int begin = 0;
	int end = 0;
	int e = 0;
	
	while((end = indexOf(exp, begin + 1)) != -1) {
		tmp.push_back(substring(begin + e, end));
		begin = end + strlen;
		e = strlen;
	}
	tmp.push_back(substring(begin, size()));
	
	return tmp;
}

OList<OString> OString::split(const string& exp) const {
	OList<OString> tmp;
	
	int strlen = exp.length();
	
	int begin = 0;
	int end = 0;
	int e = 0;
	
	while((end = indexOf(exp, begin + 1)) != -1) {
		tmp.push_back(substring(begin + e, end));
		begin = end + strlen;
		e = strlen;
	}
	tmp.push_back(substring(begin, size()));
	
	return tmp;
}

OList<OString> OString::split(const char c) const {
	OList<OString> tmp;
	
	int begin = 0;
	int end = 0;
	int e = 0;
	
	while((end = indexOf(c, begin + e)) != -1) {
		tmp.push_back(substring(begin + e, end));
		begin = end;
		e = 1;
	}
	tmp.push_back(substring(begin+e, size()));
	
	return tmp;
}

int OString::indexOf(const char in, int start) const {
	return find(in, start);
}

int OString::indexOf(const OString str, int start) const {
	return find(str, start);
}

int OString::indexOf(const char* str, int start) const {
	return find(str, start);
}

bool OString::contains(const char *str) const {
    return contains(OString(str));
}

bool OString::contains(const OString &str) const {
   return this->indexOf(str) != -1;
}

int OString::instancesOf(const char* str) const {
	int strlen = 0;
	const char* strptr = str;
	while(*strptr++)
		strlen++;
	
	int instance = 0;
	int start = 0;
	
	while((start = find(str, start)) != -1) {
		instance++;
		start += strlen;
	}
	
	return instance;
}

int OString::instancesOf(const OString str) const {
	int strlen = str.length();
	
	int instance = 0;
	int start = 0;
	
	while((start = find(str, start)) != -1) {
		instance++;
		start += strlen;
	}
	
	return instance;
}

int OString::instancesOf(const char c) const {
	int strlen = 1;
	
	int instance = 0;
	int start = 0;
	
	while((start = find(c, start)) != -1) {
		instance++;
		start += strlen;
	}
	
	return instance;
}

bool OString::startsWith(const char* str) const {
	//get a little info about the string we are comparing with
	unsigned strlen = 0;
	const char* strptr = str;
	while(*strptr++)
		strlen++;
	
	//error checking
	if(this->length() < strlen) return false;
	
	//compare
	for(unsigned i=0; i<strlen; i++) {
		if(at(i) != str[i]) return false;
	}
	return true;
}

bool OString::startsWith(const OString str) const {
	//get a little info about the string we are comparing with
	unsigned strlen = str.length();
	
	//error checking
	if(this->length() < strlen) return false;
	
	//compare
	for(unsigned i=0; i<strlen; i++) {
		if(at(i) != str[i]) return false;
	}
	return true;
}

bool OString::endsWith(const OString str) const {
	//error checking
	if(str.length() > this->length()) return false;
	
	//compare
	register int end = this->length() - str.length();
	for(register int i=this->length()-1; i>=end; i--) {
		if(this->at(i) != str.at(i - end)) {
			return false;
		}
	}
	return true;
}

bool OString::operator==(const OString& str) const {
	return this->compare(str) == 0;
}

bool OString::operator==(const char* str) const {
	return this->compare(str) == 0;
}

bool OString::operator==(const string& str) const {
	return this->compare(str) == 0;
}

bool OString::operator!=(const OString& str) const {
    return this->compare(str) != 0;
}

bool OString::operator!=(const char* str) const {
    return this->compare(str) != 0;
}

bool OString::operator>=(const OString str) const {
	return this->compare(str) >= 0;
}

bool OString::operator>=(const string& str) const {
	return this->compare(str) >= 0;
}

bool OString::operator>=(const char* str) const {
	return this->compare(str) >= 0;
}

bool OString::operator>(const OString str) const {
	return this->compare(str) > 0;
}

bool OString::operator>(const char* str) const {
	return this->compare(str) > 0;
}

bool OString::operator>(const string& str) const {
	return this->compare(str) > 0;
}

bool OString::operator<=(const OString str) const {
	return this->compare(str) <= 0;
}

bool OString::operator<=(const char* str) const {
	return this->compare(str) <= 0;
}

bool OString::operator<=(const string& str) const {
	return this->compare(str) <= 0;
}

bool OString::operator<(const OString str) const {
	return this->compare(str) < 0;
}

bool OString::operator<(const char* str) const {
	return this->compare(str) < 0;
}

bool OString::operator<(const string& str) const {
	return this->compare(str) < 0;
}

int OString::toInt(bool* ok, int base) const {
	return toLong(ok, base);
}

unsigned int OString::toUInt(bool* ok, int base) const {
	return toULong(ok, base);
}

#include<iostream>

long OString::toLong(bool* ok, int base1) const {
	if(base1 < 2 || base1 > 36) {
		if(ok) *ok = false;
		return 0;
	}
	
	int max = this->length() - 1;
	long xfer2 = 0;
	long power = 1;
	
	for(int i=max; i>=0; i--) {
		if(!isValid(at(i), base1)) {
			if(ok) *ok = false;
			return 0;
		}
		xfer2 += power * decode(at(i));
		power *= base1;
	}
	
	if(ok) *ok = 1;
	return xfer2;
}

unsigned long OString::toULong(bool* ok, int base1) const {
	if(base1 < 2 || base1 > 36) {
		if(ok) *ok = false;
		return 0;
	}
	
	int len = this->length();
	int max = len-1;
	unsigned long xfer2 = 0;
	long power = 1;
	
	for(int i=max; i>=0; i--) {
		if(!isValid(at(i), base1)) {
			if(ok) *ok = false;
			return 0;
		}
		xfer2 += power * decode(at(i));
		power *= base1;
	}
	
	if(ok) *ok = false;
	return xfer2;
}

double OString::toDouble(bool* ok) const {
	vector<OString> args = this->split('.');
	
	if(args.size() != 2) {
		if(ok) *ok = false;
		return 0;
	}
	
	bool check;
	double upperhalf = args[0].toLong(&check);
	if(!check) {
		if(ok) *ok = false;
		return 0;
	}
	
	double lowerhalf = args[1].toULong(&check);
	if(!check) {
		if(ok) *ok = false;
		return 0;
	}
	
	
	int order = orderOfMagnitude((long)lowerhalf, 10);
	for(int i=0; i<order; i++) lowerhalf /= 10.0;
	
	if(ok) *ok = true;
	if(upperhalf < 0) return upperhalf - lowerhalf;
	return upperhalf + lowerhalf;
}

OString OString::convertNum(long num, int base2) {
	long xfer2 = num;
	OString temp, out;
	while(xfer2>0) {
		temp.push_back(encode((xfer2%base2)));
		xfer2 /= base2;
	}
	int len2 = temp.length();
	for(int i=len2-1; i>=0; i--) out.push_back(temp[i]);
	
	//if the length of the string representation is 0
	//then that means there is not number so return "0"
	if(!out.length()) return "0";
	return out;
}

OString OString::convertNum(unsigned long num, int base2) {
	unsigned long xfer2 = num;
	OString temp, out;
	while(xfer2>0) {
		temp.push_back(encode((xfer2%base2)));
		xfer2 /= base2;
	}
	int len2 = temp.length();
	for(int i=len2-1; i>=0; i--) out.push_back(temp[i]);
	
	//if the length of the string representation is 0
	//then that means there is not number so return "0"
	if(!out.length()) return "0";
	return out;
}

OString OString::random(int len) {
	OString out;
	
	OTime tim;
	tim.current();

	srand(tim.usec());
	for(int i=0; i<len; i++) {
		char num = rand() % 62;
		char let;
		
		if(num < 10) let = num + '0';
		else if(num >= 10 && num < 36) let = num + 'a' - 10;
		else if(num >= 36 && num < 62) let = num + 'A' - 36;
		
		out.push_back(let);
	}
	
	return out;
}

OString OString::number(long num, int base) {
	return convertNum((long)num, base);
}

OString OString::number(unsigned long num, int base) {
	return convertNum((unsigned long)num, base);
}

OString OString::number(int num, int base) {
	return convertNum((long)num, base);
}

OString OString::number(unsigned int num, int base) {
	return convertNum((unsigned long)num, base);
}

OString OString::number(short num, int base) {
	return convertNum((long)num, base);
}

OString OString::number(unsigned short num, int base) {
	return convertNum((unsigned long)num, base);
}


OString OString::number(double num, int precision) {
	long int upperhalf = num;
	double lowertemp = (num - (double)upperhalf);
	for(int i=0; i<precision; i++) lowertemp *= 10;
	unsigned long int lowerhalf = (lowertemp < 0) ? (lowertemp * -1) : lowertemp;
	
	OString xfer;
	xfer.append(number(upperhalf));
	xfer.append('.');
	xfer.append(number(lowerhalf));
	
	while(xfer[xfer.length()-1] == '0') xfer.pop_back();
	
	return xfer;
}

int OString::orderOfMagnitude(unsigned long int num, int base) {
	int order = 0;
	while(num > 0) order++, num /= base;
	return order;
}

int OString::orderOfMagnitude(long int num, int base) {
	int order = 0;
	num = num < 0 ? num * -1 : num;
	while(num > 0) order++, num /= base;
	return order;
}

int OString::decode(int ch) {
	if(ch >= '0' && ch <= '9') return ch-'0';
	if(ch >= 'a' && ch <= 'z') return ch-'a'+10;
	if(ch >= 'A' && ch <= 'Z') return ch-'A'+10;
	return 100;
}

int OString::encode(int num) {
	if(num >= 0 && num <= 9) return num+'0';
	if(num >= 10 && num <= 35) return num+'A'-10;
	return 100;
}

bool OString::isValid(int ch, int base) {
	return (decode(ch) >= base) ? 0 : 1;
}


//**************************************************
//END OString memory management functions
//**************************************************
std::size_t hash_value(OString const& arg) {
	return arg.hashSDBM();
}

OString& operator+(const char c, OString& str) {
	str.appendFront(c);
	return str;
}

OString& operator+(const string& str1, OString& str2) {
	str2.appendFront(str1);
	return str2;
}

OString& operator+(const char* str1, OString& str2) {
	str2.appendFront(str1);
	return str2;
}

ostream& operator<<(ostream& os, const OString& str) {
	os<<str.toCString();
	return os;
}

#ifndef EOF
	#define EOF -1
#endif

istream& operator>>(istream& is, OString& str) {
	char ch;
	while((ch = is.get()) != EOF && ch != '\n') str.push_back(ch);
	return is;
}

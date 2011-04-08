#include"MainThread.h"

class baz : public OSerializable {//28
	int a;
	int b;
	int c;
	long long d;
	long long e;
	
	void serialize(OByteArray &ar, OO::ArrayBase ba) {
		ar & a;
		ar & b;
		ar & c;
		ar & d;
		ar & e;
	}
};

class bar : public OSerializable {//108
	int a;
	int b;
	long long c;
	long long d;
	baz e;
	baz f;
	baz g;
	
	void serialize(OByteArray &ar, OO::ArrayBase ba) {
		ar & a;
		ar & b;
		ar & c;
		ar & d;
		ar & e;
		ar & f;
		ar & g;
	}
};


class foo : public OSerializable {//340
	int a;
	int b;
	long long c;
	bar d;
	bar e;
	bar f;
	
	void serialize(OByteArray &ar, OO::ArrayBase ba) {
		ar & a;
		ar & b;
		ar & c;
		ar & d;
		ar & e;
		ar & f;
	}
};


int main(int argc, char *argv[]) {
	
	foo tmp;	
	cout<<"size: " <<OByteArray::sizeOf(tmp) <<endl;
	
	
	::exit(0);
	
	MainThread thread;
	thread.exec();
}

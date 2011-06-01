#include"MainThread.h"

int main(int argc, char *argv[]) {
	uint32_t i = 1;
	
	OByteArray test;
	test<<i;
	cout<<"numers" <<endl;
	test.seek(0);
	cout<<"size: " <<test.size() <<endl;
	for(int i=0; i<test.size(); i++) {
		cout<<(int)test.data()[i] <<" ";
	}cout<<endl;
	
	exit(0);
	
	MainThread thread;
	thread.exec();
}

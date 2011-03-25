#include <QtGui/QApplication>
#include "CVOperator.h"

#include<OByteArray.hpp>

int main(int argc, char *argv[]) {
	/*
	OByteArray data;
	
	
	data<<((uint8_t)123) <<((int8_t)123)
			<<(uint16_t)123 <<(int16_t)123
			<<(uint32_t)123 <<(int32_t)123
			<<(uint64_t)123 <<(int64_t)123;
	
	for(OByteArray::Iterator i=data.begin(); i<data.end(); i++) {
		cout<<(int)*((unsigned char*)i) <<" ";
	}cout<<endl;
	
	cout<<"size: " <<data.size() <<endl;
	
	data.prepend((uint8_t)123);
	data.prepend((int8_t)123);
	data.prepend((uint16_t)123);
	data.prepend((int16_t)123);
	data.prepend((uint32_t)123);
	data.prepend((int32_t)123);
	data.prepend((uint64_t)123);
	data.prepend((int64_t)123);
	
	cout<<"size: " <<data.size() <<endl;
	
	for(OByteArray::Iterator i=data.begin(); i<data.end(); i++) {
		cout<<(int)*((unsigned char*)i) <<" ";
	}cout<<endl;
	
	exit(0);
	*/
	QApplication a(argc, argv);
    CVOperator w;
    w.show();
    return a.exec();
}

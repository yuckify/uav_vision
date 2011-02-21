#include"SThread.h"

SThread::SThread() : server(this) {
  server.incommingFunc(bind(&SThread::incommingHandler, this, _1));
  server.listen(25000, "");
}

void SThread::incommingHandler(int fd) {
  cout<<"Sending Message" <<endl;
  
  GTcpSocket sock;
  sock.fileDescriptor(fd);
  
  GByteArray packet;
  packet<<"Hello";
  
  sock.write(packet);
  
  sock.waitForReadyWrite();
}
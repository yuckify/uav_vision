#include"SThread.h"

SThread::SThread() : sock(this) {
  sock.readyReadFunc(bind(&SThread::readyReadHandler, this));
  
  GNetList list = GSocket::netList().filterByName("(eth|en)[0-9]+");
  
  if(list.size() > 0)
    sock.connect(list[0].ipAddress().ipString(), 25000);
}

void SThread::readyReadHandler() {
  GByteArray pack;
  sock.readAll(pack);
  
  GString str;
  pack>>str;
  
  cout<<"Message \"" <<str <<"\"" << endl;
  
  exit(0);
}
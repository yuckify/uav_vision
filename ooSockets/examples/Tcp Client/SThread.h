#include<GByteArray.h>
#include<GTcpSocket.h>
#include<GNet.h>
#include<GThread.h>

class SThread : public GThread {
  public:
    SThread();
    
  protected:
  GTcpSocket sock;
  
  void readyReadHandler();
  
};
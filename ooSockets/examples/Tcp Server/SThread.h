#include<GTcpServer.h>
#include<GTcpSocket.h>
#include<GThread.h>
#include<GByteArray.h>

class SThread : public GThread {
  public:
    SThread();
    
    
  protected:
    GTcpServer server;
    
    void incommingHandler(int fd);
    
};
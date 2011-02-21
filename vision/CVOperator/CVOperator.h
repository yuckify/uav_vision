#ifndef CVOPERATOR_H
#define CVOPERATOR_H


#include<QMainWindow>
#include<QDockWidget>
#include<QListWidget>
#include<QFileSystemWatcher>
#include<QDir>
#include<QPixmap>
#include<QLabel>
#include<QScrollArea>
#include<QGraphicsView>
#include<QGraphicsScene>
#include<QVBoxLayout>
#include<QSocketNotifier>
#include<QLineEdit>
#include<QPushButton>

#include<iostream>
#include<memory>
#include<sys/types.h>

#include<opencv/highgui.h>
#include<opencv/cv.h>
#include<opencv/cxcore.h>

#include<OThread.hpp>
#include<OTcpSocket.hpp>
#include<OUdpSocket.hpp>
#include<OSockAddress.hpp>

#include<Defs.h>
#include<ImageDatabase.h>

using namespace std;

#define DEBUG 1

void DebugMsg(OString str);

namespace Ui {
    class CVOperator;
}

class CVOperator : public QMainWindow {
	Q_OBJECT
public:
    CVOperator(QWidget *parent = 0);
    ~CVOperator();

protected:
	//this dock widget lists the status
	//plane to cvopertator connection
	//plane to identifier
	//plane to auto pilot
	QDockWidget* statusdock;
	QVBoxLayout* statlayout;
	QLabel* cvopstat;
	QLabel* cvidstat;
	QLabel* apstat;
	
	void setCVOperatorStatus(bool stat);
	void setCVIdentifierStatus(bool stat);
	void setAutoPilotStatus(bool stat);
	
	
	//networking
	OTcpSocket* conn;
	QSocketNotifier* connNotifier;
	OUdpSocket* multlisten;
	QSocketNotifier* multNotifier;
	QSocketNotifier* multErrorNotifier;
	void multError(OSockError e);
	void connReadyRead();
	void connDisconnected();
	
	//button and input box so we can change the compression the plane
	//uses in realtime
	QLineEdit* compress;
	QPushButton* updatecomp;
public slots:
	//this function is called when the button 'updatecomp' is pressed
	void compPressed();
protected:
	
	//this is the video frame that we are currently receiving
	//that needs to be fully received before it can be uncompressed
	CvMat* compFrame;
	OString curcompext;
	uint16_t segsPerFrame;
	uint16_t align;
	int skip;
	int currentIndex;
	OByteArray prev;
	int prevlen;
	
	//this is the list of capture high resolution images on the plane
	//and the opientation of the plane at the time of capture
	ImageDatabase db;
	
private slots:
	void multActivated(int);
	void connActivated(int);
protected:
	
	//the stuff that actually displays the image in a window
	QGraphicsView* imgview;
	QGraphicsScene* imgscene;
	QLabel* disp;
	
    void changeEvent(QEvent *e);
private:
    Ui::CVOperator *ui;
	
	
};

#endif // CVOPERATOR_H

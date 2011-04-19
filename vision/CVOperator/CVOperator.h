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
#include<QTableWidget>
#include<QSettings>
#include<QTimer>
#include<QFileDialog>
#include<QFile>
#include<QTextStream>
#include<QSlider>
#include<QThread>

#include<iostream>
#include<memory>
#include<sys/types.h>
#include<assert.h>
#include<cmath>

#include<opencv/highgui.h>
#include<opencv/cv.h>
#include<opencv/cxcore.h>

#include<OThread.hpp>
#include<OTcpSocket.hpp>
#include<OUdpSocket.hpp>
#include<OSockAddress.hpp>
#include<OCompatibility.hpp>
#include<ODataStream.hpp>

#include<Defs.h>
#include<ImageDatabase.h>
#include<LogWidget.h>

using namespace std;

namespace Ui {
    class CVOperator;
}

class CVOperator : public QMainWindow {
	Q_OBJECT
public:
    CVOperator(QWidget *parent = 0);
    ~CVOperator();
	
	void closeEvent(QCloseEvent *);
	
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
	QLabel* ccstat;
	
	//saving and loading the state of the gui
	void saveSettings();
	void loadSettings();
	
	void setCVOperatorStatus(bool stat);
	void setCVIdentifierStatus(bool stat);
	void setAutoPilotStatus(bool stat);
	void setCamControlStatus(bool stat);
	
	//update the gui table for the list of images
	QTableWidget* imagetable;
	QDockWidget* itdock;
	void showImageDb(ImageDatabase& db);
	
	//networking
	ODataStream<uint32_t, uint8_t, OO::LittleEndian, true> gc;
	OTcpSocket* conn;
	QSocketNotifier* connNotifier;
	OUdpSocket* multlisten;
	QSocketNotifier* multErrorNotifier;
	void multError(OSockError e);
	void connReadyRead();
	void connDisconnected();
	void handlePacket(OByteArray& pack);
	
	//this array and these functions are for packet switching
	OList<function<void (OByteArray&)>> switcher;
	void VideoFrameHeaderSwitch(OByteArray& pack);
	void VideoFrameSegmentSwitch(OByteArray& pack);
	void ImageDetailsSwitch(OByteArray& pack);
	void ErrorMsgSwitch(OByteArray& pack);
	void CameraStatusSwitch(OByteArray& pack);
	
	//these variables are to recover is the data becomes misaligned
	OSockAddress addr;
	OByteArray alignbuffer;
	int aligned;
	int last_search;
	int source_socket;
	int found_align;
	int bps;
	QTimer* bpstimeout;
	QLabel* bpslabel;
	
	//variables to convert iplimage to qpixmap
	bool iplconv_init;
	IplImage* dframe;
	// the individual channels for the IplImage
	IplImage* tchannel0;
	IplImage* tchannel1;
	IplImage* tchannel2;
	IplImage* tchannel3;
	
	//display of the camera state
	QLabel* camstatelabel;
	
	//log widget for logging stuff
	LogWidget log;
	QDockWidget* logdock;
	
	//button and input box so we can change the compression the plane
	//uses in realtime
	QLineEdit* compress;
	QPushButton* updatecomp;
	QSlider* zoomslider;
	int zoompos;
	
	QPushButton* buttonrecord;
	CvVideoWriter* video;
	QString videofn;
	QAction* videots;
	CvSize framesize;
	
public slots:
	void recordPress();
	void setVideoFn();
	void setVideoTs();
	
	//this function is called when the user releases the slider
	//so the zoom value may be uploaded to the plane
	void zoomSliderSet(int val);
	
	//this function is called when the button 'updatecomp' is pressed
	void compPressed();
	
	//when the timer times out this function is called, a signal is
	//send to the plane request to update the image db on the ground
	void updateImageDb();
	
	//callbacks for the buttons on the log dock
	void logClear();
	void logSave();
	
	//this function is call to calculate the current data rate
	void calcData();
	
	//callbacks for controlling the camera, callbacks are called when
	//the respective button is pressed
	void camera_zin();
	void camera_zout();
	void camera_capture();
	void camera_power();
	void camera_download();
	
	//this function is for sending small messages that requires no
	//real payload, the message is composed of just the header
	void smallMsg(int in);
	
	//callbacks for options in "File" in the menubar
	void file_quit();
	
	//callbacks for options in "window" in the menubar
	void window_status();
	void window_images();
	void window_log();
	void status_vis(bool i);
	void images_vis(bool i);
	void log_vis(bool i);
protected:
	QMenu* menu_window;
	QAction* action_images;
	QAction* action_status;
	QAction* action_log;
	
	QMenu* menu_file;
	QAction* action_quit;
	
	//this is the video frame that we are currently receiving
	//that needs to be fully received before it can be uncompressed
	CvMat* compFrame;
	OString curcompext;
	uint16_t segsPerFrame;
	uint16_t align;
	int skip;
	int currentIndex;
	
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

#include <QtGui/QApplication>
#include "CVOperator.h"
#include<botan/botan.h>
#include<botan/auto_rng.h>
#include<botan/rsa.h>
#include<botan/x509_key.h>
#include<exception>

using namespace Botan;

#include<opencv/cv.h>
#include<opencv/highgui.h>

int main(int argc, char *argv[]) {
	/*
	OByteArray data;
	
	cvNamedWindow("test");
	
	IplImage* img = cvLoadImage("test.jpg");
	
	if(!img) {
		cout<<"could not open the image" <<endl;
	}
	
	data<<img;
	data.seek(0);
	
	cout<<"img w: " <<img->width <<" h: " <<img->height <<" c: " <<img->nChannels 
			<<" s: " <<img->widthStep <<endl;
	cout<<"size: " <<data.size() <<endl;
	
	IplImage* dest = NULL;
	
	data>>dest;
	
	::cvShowImage("test", dest);
	
	cvWaitKey(0);
	
	exit(0);
	*/
	
	/*
	// everyone does:
	Botan::AutoSeeded_RNG rng;
	
	// Alice
	RSA_PrivateKey priv_rsa(rng, 1024);
	std::string alice_pem = X509::PEM_encode(priv_rsa);
	
	cout<<"pem: " <<endl <<alice_pem <<endl;
	
	// send alice_pem to Bob, who does
	// Bob
	SecureVector<byte> pub_key;
	pub_key.copy((const unsigned char*)alice_pem.c_str(), alice_pem.length());
	pub_key.append(0);
	
	std::auto_ptr<X509_PublicKey> alice(X509::load_key(pub_key));
//	std::auto_ptr<X509_PublicKey> alice(X509::load_key(alice_key));
	RSA_PublicKey* alice_rsa = dynamic_cast<RSA_PublicKey*>(alice.get());
	if(alice_rsa)
	{
	
	}
	
	exit(22);
	*/
	
	
	QApplication a(argc, argv);
    CVOperator w;
    w.show();
    return a.exec();
}

#include <QtGui/QApplication>
#include "CVOperator.h"
<<<<<<< HEAD
=======
/*
#include<botan/botan.h>
#include<botan/auto_rng.h>
#include<botan/rsa.h>
#include<botan/x509_key.h>
#include<exception>

using namespace Botan;
*/

#include<opencv/cv.h>
#include<opencv/highgui.h>
>>>>>>> 15a61decd9f477a0505d0e896a7a9ee157f79983

int main(int argc, char *argv[]) {
	QApplication a(argc, argv);
    CVOperator w;
    w.show();
    return a.exec();
}

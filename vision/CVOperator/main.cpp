#include <QtGui/QApplication>
#include "CVOperator.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    CVOperator w;
    w.show();
    return a.exec();
}

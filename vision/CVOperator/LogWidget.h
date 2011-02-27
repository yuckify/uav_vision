#ifndef LOGWIDGET_H
#define LOGWIDGET_H

#include <QTextEdit>
#include<QTextStream>

#include<iostream>
#include<ostream>
#include<sys/types.h>

#include<OString.hpp>

using namespace std;

enum LogStatus {
	error,
	normal
};

class LogWidget : public QTextEdit {
	Q_OBJECT
public:
	explicit LogWidget(QObject *parent = 0);
	
	void append(const char* str);
	void append(QString str);
	void append(OString str);
	void append(uint8_t i);
	void append(uint16_t i);
	void append(uint32_t i);
	void append(int8_t i);
	void append(int16_t i);
	void append(int32_t i);
	void append(float i);
	void append(double i);
	void append(ostream& (*pf)(ostream&));
	void append(LogStatus st);
	
	LogWidget& operator<<(ostream& (*pf)(ostream&));
	
	/*
	template <class T>
			LogWidget& operator<<(T input) {
		append(input);
		return *this;
	}
*/	
	
	
signals:
	
public slots:
	
protected:
	QString line;
	
};

template <class T> LogWidget&
		operator<<(LogWidget& log, T in) {
	log.append(in);
	return log;
}

#endif // LOGWIDGET_H

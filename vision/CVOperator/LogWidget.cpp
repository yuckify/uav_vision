#include "LogWidget.h"

LogWidget::LogWidget(QObject *parent) : QTextEdit((QWidget*)parent) {
	this->setReadOnly(true);
	
	line.append(QDateTime::currentDateTime().toString("ddd MMM dd, h:m:s AP : "));
	
}

void LogWidget::append(const char* str) {
	line.append(QString::fromAscii(str));
}

void LogWidget::append(QString str) {
	line.append(str);
}

void LogWidget::append(OString str) {
	line.append(str.c_str());
}

void LogWidget::append(uint8_t i) {
	line.append(QString::number(i));
}

void LogWidget::append(uint16_t i) {
	line.append(QString::number(i));
}

void LogWidget::append(uint32_t i) {
	line.append(QString::number(i));
}

void LogWidget::append(int8_t i) {
	line.append(QString::number(i));
}

void LogWidget::append(int16_t i) {
	line.append(QString::number(i));
}

void LogWidget::append(int32_t i) {
	line.append(QString::number(i));
}

void LogWidget::append(float i) {
	line.append(QString::number(i));
}

void LogWidget::append(double i) {
	line.append(QString::number(i));
}

void LogWidget::append(ostream& (*fp)(ostream&)) {
	if(fp == (ostream& (*)(ostream&))endl) {
		QTextEdit::append(line);
		line.clear();
		line.append(QDateTime::currentDateTime().toString("ddd MMM dd, h:m:s AP : "));
		append(normal);
	}
}

void LogWidget::append(LogStatus st) {
	switch(st) {
	case error: {
			QColor tcolor;
			tcolor.setRgb(255, 0, 0);
			this->setTextColor(tcolor);
			break;
		}
	case normal: {
			QColor tcolor;
			tcolor.setRgb(0, 0, 0);
			this->setTextColor(tcolor);
			break;
		}
	case high: {
			QColor tcolor;
			tcolor.setRgb(0, 0, 255);
			this->setTextColor(tcolor);
			break;
		}
	}
}

LogWidget& LogWidget::operator<<(ostream& (*pf)(ostream&)) {
	append(pf);
}

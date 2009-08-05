#include "console.h"
#include <QPalette>

Console::Console(QWidget *parent) : QWidget(parent) 
{
	ui.setupUi(this);
	setWindowTitle("Debug Console");
	show();
}

void Console::appendMsg(const QString &xml, QtMsgType type)
{
	QString color;
	QString msgtype;
	switch (type) {
	case QtDebugMsg:
		color = "#B2B2B2";
		msgtype = "Debug";
		break;
	case QtWarningMsg:
		color = "#B2B2B2";
		msgtype = "Warning";
		break;
	case QtCriticalMsg:
		color = "#B21717";
		msgtype = "Critical";
		break;
	case QtFatalMsg:
		color = "#B21717";
		msgtype = "Fatal";
		break;
	}
	QString html = QString("<font color=\"%1\">%2: %3</font><br/>").arg(color).arg(msgtype).arg(Qt::escape(xml).replace("\n","<br/>"));
	ui.textBrowser->append(html);
}

void Console::closeEvent(QCloseEvent *event)
{
	showMinimized();
	event->ignore();
}

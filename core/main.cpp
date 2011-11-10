/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
****************************************************************************/

#include <QApplication>
#include <QTextCodec>
#include <QWidget>

#include <cstdlib>
#include <ctime>

#include "src/modulemanagerimpl.h"
#include <QTime>
#include <fstream>

#if defined(STATIC_IMAGE_PLUGINS)
Q_IMPORT_PLUGIN(qjpeg)
Q_IMPORT_PLUGIN(qgif)
#endif

//using namespace std;
//ofstream logfile;

//#ifdef Q_OS_SYMBIAN
//void SymbianLoggingHandler(QtMsgType type, const char *msg) {
//switch (type) {
//	case QtDebugMsg:
//		logfile << QTime::currentTime().toString().toLatin1().data() << " Debug: " << msg << "\n";
//		break;
//	case QtCriticalMsg:
//		logfile << QTime::currentTime().toString().toLatin1().data() << " Critical: " << msg << "\n";
//		break;
//	case QtWarningMsg:
//		logfile << QTime::currentTime().toString().toLatin1().data() << " Warning: " << msg << "\n";
//		break;
//	case QtFatalMsg:
//		logfile << QTime::currentTime().toString().toLatin1().data() <<  " Fatal: " << msg << "\n";
//		abort();
//	}
//}
//#endif

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	QTextCodec::setCodecForTr(QTextCodec::codecForName("utf-8"));
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("utf-8"));

#ifdef Q_WS_MAEMO_5
	QWidget *loadingWindow = new QWidget();
	loadingWindow->setAttribute(Qt::WA_Maemo5AutoOrientation, true);
	loadingWindow->setAttribute(Qt::WA_Maemo5ShowProgressIndicator, Qt::Checked);
	loadingWindow->show();
	app.processEvents();
#endif

//#ifdef Q_OS_SYMBIAN
//	logfile.open("c:\\data\\logfile.txt", ios::app);
//	qInstallMsgHandler(SymbianLoggingHandler);
//#endif

	Core::ModuleManagerImpl core_init;

	// At first time use current time with pointers to initiators
	qsrand(uint(std::time(0)) ^ (qHash(&core_init) ^ (qHash(&app))));
	// At second random value
	qsrand(uint(qrand()));
	// It looks like Qt doesn't always use srand as backend of qsrand
	srand(uint(qrand()));

#ifdef Q_WS_MAEMO_5
	loadingWindow->close();
#endif
	return app.exec();
}


/****************************************************************************
 *  main.cpp
 *
 *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *                2009 by Sidorov Aleksey <sauron@citadelspb.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#include <QApplication>
#include <QTextCodec>
#include <QWidget>

#include <cstdlib>
#include <ctime>

#ifdef QUTIM_DECLARATIVE_UI
#include "src/declarative/modulemanagerimpl.h"
#else
#include "src/widgets/modulemanagerimpl.h"
#endif
#include <QTime>

#if defined(STATIC_IMAGE_PLUGINS)
Q_IMPORT_PLUGIN(qjpeg)
Q_IMPORT_PLUGIN(qgif)
#endif

#ifdef TODO__MEEGO_EDITION_HARMATTAN
#include <MDeclarativeCache>

Q_DECL_EXPORT
#endif
int main(int argc, char *argv[])
{
#ifdef TODO__MEEGO_EDITION_HARMATTAN
	QScopedPointer<QApplication> app(MDeclarativeCache::qApplication(argc, argv));
#else
	QScopedPointer<QApplication> app(new QApplication(argc, argv));
#endif

	QTextCodec::setCodecForTr(QTextCodec::codecForName("utf-8"));
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("utf-8"));

#ifdef Q_WS_MAEMO_5
	QWidget *loadingWindow = new QWidget();
	loadingWindow->setAttribute(Qt::WA_Maemo5AutoOrientation, true);
	loadingWindow->setAttribute(Qt::WA_Maemo5ShowProgressIndicator, Qt::Checked);
	loadingWindow->show();
	app.processEvents();
#endif

	Core::ModuleManagerImpl core_init;

	// At first time use current time with pointers to initiators
	qsrand(uint(std::time(0)) ^ qHash(qApp));
	// At second random value
	qsrand(uint(qrand()));
	// It looks like Qt doesn't always use srand as backend of qsrand
	srand(uint(qrand()));

#ifdef Q_WS_MAEMO_5
	loadingWindow->close();
#endif
	return app->exec();
}

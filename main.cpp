/*
    main.cpp

    Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
                  2009 by Aleksey Sidorov <sauron@citadelspb.com>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/

#include <QApplication>
#include <QtCrypto>

#include <QTextCodec>
#include "src/modulemanagerimpl.h"
#include "libqutim/cryptoservice.h"
#include "libqutim/configbase.h"

#if defined(STATIC_IMAGE_PLUGINS)
Q_IMPORT_PLUGIN(qjpeg)
Q_IMPORT_PLUGIN(qgif)
#endif

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	QTextCodec::setCodecForTr(QTextCodec::codecForName("utf-8"));
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("utf-8"));
	QCA::Initializer qca_init;
	Core::ModuleManagerImpl core_init;
	// At first time use current time with pointers to initiators
	qsrand(uint(time(0)) ^ (qHash(&qca_init) ^ qHash(&core_init)));
	// At second random value
	qsrand(uint(qrand()));
	// It looks like Qt doesn't always use srand as backend of qsrand
	srand(uint(qrand()));
	return app.exec();
}

#ifdef Q_WS_WIN
#include <windows.h>
int __stdcall WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	LPWSTR *arg_w;
	int argc;
	arg_w = CommandLineToArgvW(GetCommandLineW(), &argc);
	char **argv = new char *[argc];
	for(int i=0;i<argc;i++)
	{
		int len=0;
		while(!arg_w[i][len])
			len++;
		QByteArray data = QString::fromUtf16((ushort *)arg_w[i],len).toLocal8Bit();
		argv[i] = (char *)qMalloc(data.size());
		qMemCopy(argv[i], data.data(), data.size());
	}
	return main(argc, argv);
}
#endif

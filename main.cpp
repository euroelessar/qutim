/*
    main.cpp

    Copyright (c) 2008 by Rustam Chakin <qutim.develop@gmail.com>
                  2009 by Nigmatullin Ruslan <euroelessar@gmail.com>

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
#include <QStyleFactory>
#include <qtlocalpeer.h>

#include "src/qutim.h"
#include <QString>
#include <QByteArray>

#if defined(Q_OS_UNIX)
#include <signal.h>

void sig_quit(int s)
{
    QCoreApplication::quit();
}
#endif

#if defined(STATIC_IMAGE_PLUGINS)
Q_IMPORT_PLUGIN(qjpeg)
Q_IMPORT_PLUGIN(qgif)
#endif

#include "src/cryptoserviceimpl.h"

int main(int argc, char *argv[])
{
    #if defined(Q_OS_UNIX)
    signal(SIGINT,sig_quit);
    signal(SIGHUP,sig_quit);
	#endif

	// Make copy of arguments, because Qt docs says that Qt may change them
	int argc_ = argc;
	char **argv_ = (char **)qMalloc( sizeof(argv) );
	for( int i=0; i<argc; i++ )
	{
		int length = ( qstrlen(argv[i]) + 1 ) * sizeof(char);
		argv_[i] = (char *)qMalloc( length );
		qMemCopy( argv_[i], argv[i], length );
	}

	// Create instance of QApplication, without it Oxygen style
	// doesn't work properly with kde-integration plugins loaded
	new QApplication(argc_, argv_);

	if ( QFile::exists(QCoreApplication::applicationDirPath() + "/config"))
		QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, QCoreApplication::applicationDirPath() + "/config");
	QSettings::setDefaultFormat(QSettings::IniFormat);
	QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim", "qutimsettings");
	bool single_app = settings.value( "General/single", true ).toBool();

	QPointer<QtLocalPeer> peer;
	if( single_app )
	{
		peer = QPointer<QtLocalPeer>( new QtLocalPeer(qApp) );
		if( peer->isClient() )
		{
			qWarning( "\"%s\" is already started", qPrintable(peer->applicationId()) );
			if( peer->sendMessage( "awake!", 1000 ) )
			{
				delete qApp;
				return 0;
			}
		}
	}
	QCA::Initializer init;

	QObjectList plugins	= PluginSystem::instance().loadPlugins( argc, argv );
	// May be some of plugins deleted QApplication? Who knows..
	// I think that plugin has a reason for it, so we quit
	// For example if plugin looks for only one instance of qutIM loaded in the moment
	if( !qApp)
		return 0;
	if( single_app )
	{
		if( !peer )
			peer = QPointer<QtLocalPeer>( new QtLocalPeer(qApp) );
		if( peer->isClient() )
		{
			qWarning( "\"%s\" is already started", qPrintable(peer->applicationId()) );
			if( peer->sendMessage( "awake!", 1000 ) )
			{
				delete qApp;
				return 0;
			}
		}
		QObject::connect( peer, SIGNAL(messageReceived(QString)), &PluginSystem::instance(), SLOT(showCL()) );
	}

	PluginSystem::instance().init();

	qApp->setQuitOnLastWindowClosed(false);
	qutIM w(plugins);

	int result = 0;

	if (w.isShouldRun())		// User decide to sign in
		result = QApplication::exec();

	delete qApp;				// Clean up resources
	return result;
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

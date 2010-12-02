#include "crashplugin.h"
#include <signal.h>
#include <kglobal.h>
#include <QApplication>
#include <shared/shareddata.h>
#include <kdeversion.h>

KdeCrashHandler::KdeCrashHandler()
{
	KCrash::setApplicationPath( qApp->applicationDirPath() );
	KCrash::setApplicationName( qAppName() );
#if KDE_VERSION_MINOR < 5
	KCrash::setCrashHandler(KCrash::defaultCrashHandler);
#else
	KCrash::setDrKonqiEnabled (true);
#endif
}

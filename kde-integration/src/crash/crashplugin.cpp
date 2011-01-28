#include "crashplugin.h"
#include <signal.h>
#include <kglobal.h>
#include <QApplication>
#include <shared/shareddata.h>
#include <kdeversion.h>

KdeCrashHandler::KdeCrashHandler()
{
#if (KDE_VERSION_MINOR < 6)
	// this asserts with kde 4.6 beta 2 with no apparent reason
	KCrash::setApplicationPath( qApp->applicationDirPath() );
#endif
	KCrash::setApplicationName( qAppName() );
#if KDE_VERSION_MINOR < 5
	KCrash::setCrashHandler(KCrash::defaultCrashHandler);
#else
	KCrash::setDrKonqiEnabled (true);
#endif
}

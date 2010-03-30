#include "crashplugin.h"
#include <signal.h>
#include <kglobal.h>
#include <QApplication>
#include <shared/shareddata.h>

KdeCrashHandler::KdeCrashHandler()
{
	KCrash::setApplicationPath( qApp->applicationDirPath() );
	KCrash::setApplicationName( qAppName() );
	KCrash::setCrashHandler( KCrash::defaultCrashHandler );
}

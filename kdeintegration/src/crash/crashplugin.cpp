#include "crashplugin.h"
#include <signal.h>
#include <kglobal.h>
#include <QApplication>
#include <shared/shareddata.h>
#include <kdeversion.h>

KdeCrashHandler::KdeCrashHandler()
{
	KCrash::setApplicationPath(QCoreApplication::applicationDirPath());
	KCrash::setApplicationName(qAppName());
	if (KDE::versionMinor() < 5)
		KCrash::setCrashHandler(KCrash::defaultCrashHandler);
#if KDE_VERSION_MINOR >= 5
	else
		KCrash::setDrKonqiEnabled(true);
#endif
}

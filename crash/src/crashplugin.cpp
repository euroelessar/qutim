#include "crashplugin.h"
#include <signal.h>
#include <kglobal.h>
#include <QApplication>
#include <shared/shareddata.h>

bool CrashPlugin::init( PluginSystemInterface *plugin_system )
{
	KCrash::setApplicationPath( qApp->applicationDirPath() );
	KCrash::setApplicationName( qAppName() );
	KCrash::setCrashHandler( KCrash::defaultCrashHandler );
	PluginInterface::init(plugin_system);
	KdeIntegration::ensureActiveComponent();
	return true;
}

void CrashPlugin::release()
{
}

Q_EXPORT_PLUGIN2(crash, CrashPlugin)

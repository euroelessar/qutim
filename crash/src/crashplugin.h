#ifndef CRASHPLUGIN_H
#define CRASHPLUGIN_H

#include <QObject>
#include <qutim/plugininterface.h>
#include <kcrash.h>

using namespace qutim_sdk_0_2;

class CrashPlugin : public QObject, public PluginInterface {
	Q_OBJECT
	Q_INTERFACES(qutim_sdk_0_2::PluginInterface)
public:
	virtual bool init( PluginSystemInterface *plugin_system );
	virtual void release();
	virtual void setProfileName( const QString &profile_name ) { Q_UNUSED(profile_name); }
	virtual QString name() { return "KdeCrash"; }
	virtual QString description() { return "Plugins for handling qutIM's craches by KCrash."; }
	virtual QIcon *icon() { return 0; }
	virtual QString type() { return "crashhandler"; }
};

#endif // CRASHPLUGIN_H

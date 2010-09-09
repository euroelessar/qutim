#ifndef GROWLPLUGIN_H
#define GROWLPLUGIN_H

#include <QObject>
#include <qutim/plugin.h>

class GrowlPlugin : public qutim_sdk_0_3::Plugin
{
    Q_OBJECT
public:
	virtual void init();
	virtual bool load();
	virtual bool unload();
	virtual ~GrowlPlugin() {}
};

#endif // GROWLPLUGIN_H

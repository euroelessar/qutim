#ifndef KDEEMOTICONSPLUGIN_H
#define KDEEMOTICONSPLUGIN_H

#include "kdeemoticonslayer.h"

using namespace qutIM;

class KdeEmoticonsPlugin : public QObject, public LayerPluginInterface
{
	Q_OBJECT
	Q_INTERFACES(qutim_sdk_0_2::PluginInterface qutim_sdk_0_2::LayerPluginInterface)
public:
	KdeEmoticonsPlugin( QObject *parent = 0 );
	virtual bool init( PluginSystemInterface *plugin_sustem );
	virtual void release();
	virtual void setProfileName( const QString &profile_name ) { Q_UNUSED(profile_name); }
	virtual QString name() { return "KDEEmoticons"; }
	virtual QString description() { return "Plugins for integrating qutIM's emoticons engine with KDE."; }
	virtual QIcon *icon() { return 0; }
private:
	KdeEmoticonsLayer *m_layer;
};

#endif // KDEEMOTICONSPLUGIN_H

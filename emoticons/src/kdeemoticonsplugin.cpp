#include "kdeemoticonsplugin.h"
#include <shared/shareddata.h>

KdeEmoticonsPlugin::KdeEmoticonsPlugin( QObject *parent ) : QObject(parent)
{
	m_layer = 0;
}

bool KdeEmoticonsPlugin::init( PluginSystemInterface *plugin_system )
{
	PluginInterface::init(plugin_system);
	KdeIntegration::ensureActiveComponent();
	m_layer = new KdeEmoticonsLayer();
	if( plugin_system->setLayerInterface( EmoticonsLayer, m_layer ) )
		return true;
	delete m_layer;
	m_layer = 0;
	return false;
}

void KdeEmoticonsPlugin::release()
{
	if( m_layer )
	{
		m_layer->release();
		delete m_layer;
		m_layer = 0;
	}
}

Q_EXPORT_PLUGIN2(kdeemoticons, KdeEmoticonsPlugin)

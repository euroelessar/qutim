/****************************************************************************
 * kdespellerplugin.cpp
 *
 *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#include "kdespellerplugin.h"
#include <shared/shareddata.h>


KdeSpellerPlugin::KdeSpellerPlugin(QObject *parent) :
	QObject(parent)
{
}

bool KdeSpellerPlugin::init( PluginSystemInterface *plugin_system )
{
	PluginInterface::init(plugin_system);
	KdeIntegration::ensureActiveComponent();
	m_layer = new KdeSpellerLayer();
	if( plugin_system->setLayerInterface( SpellerLayer, m_layer ) )
		return true;
	delete m_layer;
	m_layer = 0;
	return false;
}

void KdeSpellerPlugin::release()
{
	if( m_layer )
	{
		m_layer->release();
		delete m_layer;
		m_layer = 0;
	}
}

Q_EXPORT_PLUGIN2(kdespeller, KdeSpellerPlugin)

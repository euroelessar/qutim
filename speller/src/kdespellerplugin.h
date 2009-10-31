/****************************************************************************
 * kdespellerplugin.h
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

#ifndef KDESPELLERPLUGIN_H
#define KDESPELLERPLUGIN_H

#include "kdespellerlayer.h"

using namespace qutIM;

class KdeSpellerPlugin : public QObject, public LayerPluginInterface
{
	Q_OBJECT
	Q_INTERFACES(qutim_sdk_0_2::PluginInterface qutim_sdk_0_2::LayerPluginInterface)
public:
	KdeSpellerPlugin(QObject *parent = 0);
	virtual bool init( PluginSystemInterface *plugin_sustem );
	virtual void release();
	virtual void setProfileName( const QString &profile_name ) { Q_UNUSED(profile_name); }
	virtual QString name() { return "KDESpeller"; }
	virtual QString description() { return "Plugins for integrating qutIM's speller engine with KDE."; }
	virtual QIcon *icon() { return 0; }
private:
	KdeSpellerLayer *m_layer;
};

#endif // KDESPELLERPLUGIN_H

/****************************************************************************
 * ulplugin.h
 *  Copyright © 2011, Vsevolod Velichko<torkvema@gmail.com>.
 *  Licence: GPLv2 or later
 *
 ****************************************************************************
 *                                                                          *
 *   This library is free software; you can redistribute it and/or modify   *
 *   it under the terms of the GNU General Public License as published by   *
 *   the Free Software Foundation; either version 2 of the License, or      *
 *   (at your option) any later version.                                    *
 *                                                                          *
 ****************************************************************************/

#ifndef ULPLUGIN_HGWBQ28Y
#define ULPLUGIN_HGWBQ28Y

#include <qutim/plugin.h>
#include <QWeakPointer>
#include "ulservice.h"

class UnityLauncherPlugin : public qutim_sdk_0_3::Plugin
{
	Q_OBJECT
public:
	explicit UnityLauncherPlugin ();
	virtual void init();
	virtual bool load();
	virtual bool unload();
private:
	QWeakPointer<UnityLauncherService> service;
};

#endif /* end of include guard: ULPLUGIN_HGWBQ28Y */

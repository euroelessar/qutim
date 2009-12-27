/****************************************************************************
 *  oscarplugin.h
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

#ifndef OSCARPLUGIN_H
#define OSCARPLUGIN_H

#include <qutim/plugin.h>
#include "ui/icqmainsettings.h"
#include "qutim/settingslayer.h"

namespace Icq
{

using namespace qutim_sdk_0_3;

class OscarPlugin: public Plugin
{
	Q_OBJECT
public:
	OscarPlugin();
	void init();
	bool load();
	bool unload();
};

} // namespace Icq

#endif // OSCARPLUGIN_H

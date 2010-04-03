/****************************************************************************
 * aspellplugin.h
 *
 *  Copyright (c) 2010 by Ruslan Nigmatullin <euroelessar@gmail.com>
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

#ifndef ASPELLPLUGIN_H
#define ASPELLPLUGIN_H

#include <qutim/plugin.h>

using namespace qutim_sdk_0_3;

class ASpellPlugin : public Plugin
{
Q_OBJECT
public:
	explicit ASpellPlugin();
	virtual void init();
	virtual bool load();
	virtual bool unload();
};

#endif // ASPELLPLUGIN_H

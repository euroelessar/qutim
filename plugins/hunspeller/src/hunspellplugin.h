/****************************************************************************
 *  hunspellplugin.h
 *
 *  Copyright (c) 2010 by Alexey Prokhin <alexey.prokhin@yandex.ru>
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

#ifndef HUNSPELLPLUGIN_H
#define HUNSPELLPLUGIN_H

#include <qutim/plugin.h>

using namespace qutim_sdk_0_3;

class HunSpellPlugin : public Plugin
{
Q_OBJECT
public:
	explicit HunSpellPlugin();
	virtual void init();
	virtual bool load();
	virtual bool unload();
};

#endif // HUNSPELLPLUGIN_H

/****************************************************************************
 *  vkontakteplugin.h
 *
 *  Copyright (c) 2010 by Aleksey Sidorov <sauron@citadelspb.com>
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

#ifndef VKONTAKTEPLUGIN_H
#define VKONTAKTEPLUGIN_H
#include <qutim/plugin.h>
#include "vkontakte_global.h"

class VkontaktePlugin : public Plugin
{
	Q_OBJECT
public:
	virtual void init();
	virtual bool load();
	virtual bool unload();
};

#endif // VKONTAKTEPLUGIN_H

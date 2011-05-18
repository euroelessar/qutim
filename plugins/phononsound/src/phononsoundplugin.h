/****************************************************************************
 *
 *  This file is part of qutIM
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This file is part of free software; you can redistribute it and/or    *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************
 ****************************************************************************/

#ifndef FLOATIES_H
#define FLOATIES_H

#include <qutim/plugin.h>

using namespace qutim_sdk_0_3;

class PhononSoundPlugin : public Plugin
{
	Q_OBJECT
public:
	virtual void init();
	virtual bool load();
	virtual bool unload();
};

#endif // FLOATIES_H

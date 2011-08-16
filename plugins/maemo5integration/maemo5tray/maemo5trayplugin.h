/****************************************************************************
 *  maemo5trayplugin.h
 *
 *  Copyright (c) 2011 by Evgeniy Degtyarev <degtep@gmail.comu>
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

#ifndef MAEMO5TRAYPLUGIN_H
#define MAEMO5TRAYPLUGIN_H

#include <qutim/plugin.h>

using namespace qutim_sdk_0_3;

class Maemo5TrayPlugin : public Plugin
{
Q_OBJECT
public:
	explicit Maemo5TrayPlugin();
	virtual void init();
	virtual bool load();
	virtual bool unload();
};

#endif // MAEMO5TRAYPLUGIN_H

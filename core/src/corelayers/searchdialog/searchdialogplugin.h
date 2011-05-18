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

#ifndef SEARCHDIALOGPLUGIN_H
#define SEARCHDIALOGPLUGIN_H

#include <qutim/plugin.h>

namespace Core
{
	class SearchDialogPlugin : public qutim_sdk_0_3::Plugin
	{
		Q_OBJECT
	public:
		virtual void init();
		virtual bool load();
		virtual bool unload();
	};
}

#endif // SEARCHDIALOGPLUGIN_H

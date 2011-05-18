/****************************************************************************
 *  oscarplugin.h
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *                        Prokhin Alexey <alexey.prokhin@yandex.ru>
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

namespace qutim_sdk_0_3 {

namespace oscar {

class OscarPlugin: public Plugin
{
	Q_OBJECT
	Q_CLASSINFO("DebugName", "ICQ")
public:
	OscarPlugin();
	void init();
	bool load();
	bool unload();
};

} } // namespace qutim_sdk_0_3::oscar

#endif // OSCARPLUGIN_H

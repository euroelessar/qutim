/****************************************************************************
 *  ircplugin.h
 *
 *  Copyright (c) 2010 by Prokhin Alexey <alexey.prokhin@yandex.ru>
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

#ifndef IRCPLUGIN_H
#define IRCPLUGIN_H

#include <qutim/plugin.h>

namespace qutim_sdk_0_3 {

namespace irc {

class IrcPlugin: public Plugin
{
	Q_OBJECT
	Q_CLASSINFO("DebugName", "IRС")
public:
	IrcPlugin();
	void init();
	bool load();
	bool unload();
};

} } // namespace qutim_sdk_0_3::irc

#endif // IRCPLUGIN_H

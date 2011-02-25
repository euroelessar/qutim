/****************************************************************************
 *  simpleactionsplugin.h
 *
 *  Copyright (c) 2011 by Sidorov Aleksey <sauron@citadelspb.com>
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

#ifndef SIMPLEACTIONSPLUGIN_H
#define SIMPLEACTIONSPLUGIN_H
#include <qutim/plugin.h>

namespace Core {

class SimpleActions;
class SimpleActionsPlugin : public qutim_sdk_0_3::Plugin
{
    Q_OBJECT
public:
	SimpleActionsPlugin();
	virtual void init();
	virtual bool load();
	virtual bool unload();
private:
	SimpleActions *m_actions;
};

} // namespace Core

#endif // SIMPLEACTIONSPLUGIN_H

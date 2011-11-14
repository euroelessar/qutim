/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Alexey Prokhin <alexey.prokhin@yandex.ru>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
****************************************************************************/

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


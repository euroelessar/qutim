/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Evgeniy Degtyarev <degtep@gmail.com>
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

#ifndef MAEMO5LEDPLUGIN_H
#define MAEMO5LEDPLUGIN_H

#include <qutim/plugin.h>

using namespace qutim_sdk_0_3;

class Maemo5LedPlugin : public Plugin
{
	Q_OBJECT
public:
	explicit Maemo5LedPlugin();
	virtual void init();
	virtual bool load();
	virtual bool unload();
};

#endif // MAEMO5LEDPLUGIN_H


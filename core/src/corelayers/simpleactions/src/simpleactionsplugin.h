/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Aleksey Sidorov <gorthauer87@yandex.ru>
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

#ifndef SIMPLEACTIONSPLUGIN_H
#define SIMPLEACTIONSPLUGIN_H
#include <qutim/plugin.h>

namespace Core {

class SimpleActions;
class SimpleActionsPlugin : public qutim_sdk_0_3::Plugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qutim.Plugin")
	Q_CLASSINFO("Uses", "ContactInfo")
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


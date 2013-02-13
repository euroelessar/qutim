/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2013 Tretyakov Roman <roman@trett.ru>
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

#ifndef AUTOPASTER_H
#define AUTOPASTER_H

#include <qutim/plugin.h>
#include "handler.h"


class AutoPaster : public qutim_sdk_0_3::Plugin
{
	Q_OBJECT
	Q_CLASSINFO("DebugName","Autopaster")
public:
	virtual bool load();
	virtual bool unload();
	virtual void init();

protected slots:
	void loadSettings();

private:
	AutoPasterHandler *m_handler;
	qutim_sdk_0_3::SettingsItem *m_settingsItem;
};

#endif // AUTOPASTER_H

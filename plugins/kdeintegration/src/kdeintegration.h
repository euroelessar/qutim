/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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
#ifndef KDEINTEGRATION_H
#define KDEINTEGRATION_H

#include <qutim/plugin.h>
#include <khelpmenu.h>

using namespace qutim_sdk_0_3;

namespace KdeIntegration
{
class KdePlugin : public Plugin
{
	Q_OBJECT
public:
	KdePlugin();
	virtual void init();
	virtual bool load();
	virtual bool unload();
	virtual bool eventFilter(QObject *, QEvent *);

	static KHelpMenu *helpMenu();
private:
	quint16 m_quetzal_id;
};
}

#endif // KDEINTEGRATION_H


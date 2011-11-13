/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin euroelessar@yandex.ru
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

#ifndef UNITYDOCK_GN5KWDY6
#define UNITYDOCK_GN5KWDY6

#include "dockbase.h"
#include <dbusmenuexporter.h>

class QMenu;

class UnityDock : public DockBase
{
	Q_OBJECT
public:
	UnityDock(QObject *p = NULL);
	~UnityDock();

public slots:
	virtual void setIcon(const QIcon &);
	virtual void setOverlayIcon(const QIcon &);
	virtual void setMenu(QMenu *menu);
	virtual void setProgress(int progress);

	virtual void setBadge(const QString &badge);
	virtual void setCount(int count);
	virtual void setAlert(bool on = true);
private:
	template<typename T> void sendMessage(const char *name, const T& val);
	QWeakPointer<DBusMenuExporter> m_menu;
};

#endif /* end of include guard: UNITYDOCK_GN5KWDY6 */


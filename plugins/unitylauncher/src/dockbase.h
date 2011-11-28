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

#ifndef DOCKBASE_1FEA1T7G
#define DOCKBASE_1FEA1T7G

#include <QIcon>
#include <QMenu>
#include <QString>

class DockBase : public QObject
{
	Q_OBJECT
public:
	DockBase(QObject *p = NULL);
	~DockBase() = 0;
public slots:
	virtual void setIcon(const QIcon &);
	virtual void setOverlayIcon(const QIcon &);
	virtual void setMenu(QMenu *);
	virtual void setProgress(int);

	virtual void setBadge(const QString &);
	virtual void setAlert(bool on);
};


#endif /* end of include guard: DOCKBASE_1FEA1T7G */


/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Sidorov Aleksey <sauron@citadelspb.com>
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

#ifndef SERVICEITEM_H
#define SERVICEITEM_H

#include <QStandardItem>

namespace Core
{
	class ServiceItem : public QStandardItem
	{
	public:
		enum ServiceItemRole {
			ExclusiveRole = Qt::UserRole + 2,
			ExtentionInfoRole
		};
		ServiceItem(const QIcon &icon,const QString &text);
		virtual void setData(const QVariant& value, int role = Qt::UserRole + 1);
	};
}
#endif // SERVICEITEM_H


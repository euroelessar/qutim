/****************************************************************************
 *  serviceitem.h
 *
 *  Copyright (c) 2010 by Aleksey Sidorov <sauron@citadelspb.com>
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

#ifndef SERVICEITEM_H
#define SERVICEITEM_H

#include <QStandardItem>

namespace Core
{
	class ServiceItem : public QStandardItem
	{
	public:
		enum ServiceItemRole {
			ExclusiveRole = Qt::UserRole + 32,
			ClassNameRole
		};
		ServiceItem(const QIcon &icon,const QString &text);
		virtual void setData(const QVariant& value, int role = Qt::UserRole + 1);
	};
}
#endif // SERVICEITEM_H

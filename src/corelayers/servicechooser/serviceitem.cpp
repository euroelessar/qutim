/****************************************************************************
 *  serviceitem.cpp
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

#include "serviceitem.h"
#include <libqutim/icon.h>

namespace Core
{
	ServiceItem::ServiceItem(const QIcon& icon, const QString& text, bool exclusive) :
	m_exclusive(exclusive)
	{
		setText(text);
		setIcon(icon);
	}

	QVariant ServiceItem::data(int role) const
	{
		switch (role) {
			case ExclusiveRole:
				return QVariant(m_exclusive);
				break;
			case ClassNameRole:
				return QVariant(m_service_classname);
				break;
			default:
				return QStandardItem::data(role);
		}
	}
	
	void ServiceItem::setData(const QVariant& value, int role)
	{
		if (role == Qt::CheckStateRole && parent() && parent()->data(ExclusiveRole).toBool()) {
			Qt::CheckState state = static_cast<Qt::CheckState>(value.toInt());
			
			if (state == Qt::Checked) {
				for (int row = 0;row!=parent()->rowCount();row++) {
					parent()->child(row)->setData(Qt::Unchecked,role);
				}
			}
			//TODO provide a situation where none of the items will not be selected
// 			else {
// 				Qt::CheckState current_state = static_cast<Qt::CheckState>(data(Qt::CheckStateRole).toInt());
// 				if (state == Qt::Unchecked && current_state == Qt::Checked) {
// 					return;
// 				}
// 			}
		}
		
		QStandardItem::setData(value, role);
	}
	void ServiceItem::setServiceClassName(const char* name)
	{
		m_service_classname = name;
	}



}

#include "simplecontactlistdelegate.h"
#include "libqutim/tooltip.h"
#include <QToolTip>
/****************************************************************************
 *  simplecontactlistitem.cpp
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

#include "simplecontactlistitem.h"
#include <QHelpEvent>
#include <QAbstractItemView>

namespace Core
{
	namespace SimpleContactList
	{
		SimpleContactListDelegate::SimpleContactListDelegate(QObject *parent) :
				QStyledItemDelegate(parent)
		{
		}
		
		bool SimpleContactListDelegate::helpEvent(QHelpEvent *event,
					   QAbstractItemView *view,
					   const QStyleOptionViewItem &option,
					   const QModelIndex &index)
		{
			Q_UNUSED(option);
		
			if (!event || !view)
				return false;
			if (event->type() == QEvent::ToolTip) {
				QHelpEvent *he = static_cast<QHelpEvent*>(event);
				if (getItemType(index) == ContactType) {
					ContactItem *item = reinterpret_cast<ContactItem*>(index.internalPointer());
					qutim_sdk_0_3::ToolTip::instance()->showText(he->globalPos(),
																 item->data->contact, view);
					return true;
				}
				QVariant tooltip = index.data(Qt::ToolTipRole);
				if (qVariantCanConvert<QString>(tooltip)) {
					QToolTip::showText(he->globalPos(), tooltip.toString(), view);
					return true;
				}
			}
			return false;
		}
	}
}

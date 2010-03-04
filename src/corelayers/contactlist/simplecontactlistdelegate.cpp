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
#include <QApplication>
#include <libqutim/debug.h>

namespace Core
{
	namespace SimpleContactList
	{
		SimpleContactListDelegate::SimpleContactListDelegate(QObject *parent) :
				QStyledItemDelegate(parent)
		{
		}
		
		void SimpleContactListDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
		{
			QStyleOptionViewItemV4 opt(option);
			QStyle *style = opt.widget ? opt.widget->style() : QApplication::style();
			style->drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, painter, opt.widget);
			
			const QAbstractItemModel *model = index.model();
			
			ItemType type = static_cast<ItemType>(model->data(index,ItemDataType).toInt());
			
			switch (type) {
				case TagType: {
					style->drawPrimitive(QStyle::PE_PanelButtonBevel,&opt,painter,opt.widget);					
					break;
				}
				case ContactType: {
					break;
				}
				default:
					break;
			}
			
			QStyledItemDelegate::paint(painter, option, index);
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
		
		QSize SimpleContactListDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
		{
			QSize size = QStyledItemDelegate::sizeHint(option, index);
			return QSize(size.width(),24);
		}

	}
}

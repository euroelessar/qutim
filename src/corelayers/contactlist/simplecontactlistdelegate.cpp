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
#include <QPainter>

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
			QStyle *style = QApplication::style();
			
			const QAbstractItemModel *model = index.model();
			
			ItemType type = static_cast<ItemType>(model->data(index,ItemDataType).toInt());
			
			QString name = model->data(index,Qt::DisplayRole).toString();
			
			switch (type) {
				case TagType: {
					QStyleOptionViewItem opt(option);
					style->drawPrimitive(QStyle::PE_PanelButtonBevel,&opt,painter);
					break;
				}
				case ContactType: {
					QStyleOptionViewItemV4 opt(option);
					style->drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, painter, opt.widget);
					break;
				}
				default:
					break;
			}
			
			QIcon item_icon = model->data(index, Qt::DecorationRole).value<QIcon>();
			item_icon.paint(painter,
						option.rect.left() +5, //FIXME
						option.rect.top(),
						16, //FIXME
						option.rect.height(),
						Qt::AlignVCenter);
			
			painter->drawText(option.rect.left() + 26,
								option.rect.top() + 5,
								option.rect.right(),
								option.rect.height(),
								Qt::AlignTop,
								name  
							);

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

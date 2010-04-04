/****************************************************************************
 *  simplecontactlistitem.cpp
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *  Copyright (c) 2010 by Sidorov Aleksey <sauron@citadelspb.com>
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

#include "simplecontactlistdelegate.h"
#include "libqutim/tooltip.h"
#include <QToolTip>
#include "simplecontactlistitem.h"
#include <QHelpEvent>
#include <QAbstractItemView>
#include <QApplication>
#include <libqutim/debug.h>
#include <QPainter>

namespace Core
{
	static int m_vertical_padding = 3;
	static int m_horizontal_padding = 5;
	static int m_icon_size = 16;
	
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
			
			const QAbstractItemModel *model = index.model();

			if (!model)
				return;
			
			ItemType type = static_cast<ItemType>(model->data(index,ItemDataType).toInt());
			
			QString name = model->data(index,Qt::DisplayRole).toString();
			QString status_text = model->data(index,ItemStatusText).toString();
			
			switch (type) {
				case TagType: {
					style->drawPrimitive(QStyle::PE_PanelButtonBevel,&opt,painter, opt.widget);
					
					QPixmap pixmap(option.rect.size());
					pixmap.fill(Qt::transparent);				
					QPainter p(&pixmap);
					p.translate(-option.rect.topLeft());
					
					QFont font = opt.font;
					font.setBold(true);
					p.setFont(font);
					
					p.drawText(option.rect.left() + 2*m_horizontal_padding + m_icon_size,
							   option.rect.top(),
							   option.rect.right(),
							   option.rect.height(),
							   Qt::AlignLeft | Qt::AlignVCenter,
							   name  
							   );
					
					p.end();
					painter->drawPixmap(option.rect,pixmap);
					
					break;
				}
				case ContactType: {
					style->drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, painter, opt.widget);
					
					
					if (!status_text.isEmpty()) {
 						QPixmap pixmap(option.rect.size());
						pixmap.fill(Qt::transparent);				
						QPainter p(&pixmap);
						p.translate(-option.rect.topLeft());
				
						p.setPen(opt.palette.color(QPalette::Shadow));
						QFont font = opt.font;
						font.setPointSize(font.pointSize() - 1);
						p.setFont(font);
						p.drawText(option.rect.left() + 2*m_horizontal_padding + m_icon_size,
								   option.rect.top(),
								   option.rect.right(),
								   option.rect.height(),
								   Qt::AlignBottom,
								   status_text.remove("\n")
									);
						p.end();
						painter->drawPixmap(option.rect,pixmap);
						
					}
					
					painter->drawText(option.rect.left() + 2*m_horizontal_padding + m_icon_size,
									  option.rect.top() + m_vertical_padding,
									  option.rect.right(),
									  option.rect.height() - m_vertical_padding,
									  Qt::AlignTop,
									  name  
									);
					
					break;
				}
				default:
					break;
			}
			
			QIcon item_icon = model->data(index, Qt::DecorationRole).value<QIcon>();
			item_icon.paint(painter,
							option.rect.left() + m_horizontal_padding, //FIXME
							option.rect.top(),
							m_icon_size, //FIXME
							option.rect.height(),
							Qt::AlignVCenter);
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

			const QAbstractItemModel *model = index.model();

			if (!model)
				return size;

			int height = m_icon_size + 2 * m_vertical_padding;
			if (!model->data(index,ItemStatusText).toString().isEmpty())
				height += option.font.pointSize();
				
				
			height = (model->data(index,ItemDataType).toInt() == TagType) ? m_icon_size + m_vertical_padding : height;
			size.rheight() = height;	
			return size;
		}

	}
}

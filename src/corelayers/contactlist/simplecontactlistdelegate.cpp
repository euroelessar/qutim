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
	namespace SimpleContactList
	{
		SimpleContactListDelegate::SimpleContactListDelegate(QObject *parent) :
				QAbstractItemDelegate(parent)
		{
			m_horizontal_padding = 5;
			m_vertical_padding = 3;
			m_show_flags = static_cast<ShowFlags>(ShowStatusText | ShowExtendedStatusIcons);
		}
		
		void SimpleContactListDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
		{			
			QStyleOptionViewItemV4 opt(option);
			QStyle *style = opt.widget ? opt.widget->style() : QApplication::style();
			
			ItemType type = static_cast<ItemType>(index.data(ItemDataType).toInt());
			
			QString name = index.data(Qt::DisplayRole).toString();

			QFont original_font = painter->font();
			QPen original_pen = painter->pen();

			QRect title_rect = option.rect;
			title_rect.setLeft(title_rect.left() + option.decorationSize.width() + 2*m_horizontal_padding);
			title_rect.setTop(title_rect.top() + m_vertical_padding);
			title_rect.setBottom(title_rect.bottom() - m_vertical_padding);
			
			switch (type) {
			case TagType: {
					QStyleOptionButton buttonOption;

					buttonOption.state = option.state;					
#ifdef Q_WS_MAC
					buttonOption.features = QStyleOptionButton::Flat;
					buttonOption.state |= QStyle::State_Raised;
					buttonOption.state &= ~QStyle::State_HasFocus;
#endif

					buttonOption.rect = option.rect;
					buttonOption.palette = option.palette;
					style->drawControl(QStyle::CE_PushButton, &buttonOption, painter, opt.widget);
					
					QFont font = opt.font;
					font.setBold(true);
					painter->setFont(font);
					
					painter->drawText(title_rect,
									  Qt::AlignVCenter,
									  name
									  );
					break;
				}
			case ContactType: {
					style->drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, painter, opt.widget);
					QRect bounding;
					painter->drawText(title_rect,
									  Qt::AlignTop,
									  name,
									  &bounding
									  );
					if (m_show_flags & ShowStatusText) {
						Status status = index.data(ItemStatusRole).value<Status>();
						if (!status.text().isEmpty()) {
							QRect status_rect = title_rect;
							status_rect.setTop(status_rect.top() + bounding.height());
#ifdef Q_WS_MAC
							painter->setPen(opt.palette.color(QPalette::Inactive, QPalette::WindowText));
#else
							painter->setPen(opt.palette.color(QPalette::Shadow));
#endif
							QFont font = opt.font;
							font.setPointSize(font.pointSize() - 1);
							painter->setFont(font);
							painter->drawText(status_rect,
											  Qt::AlignTop | Qt::TextWordWrap,
											  status.text().remove("\n")
											  );
						}
					}
					
					break;
				}
			default:
				break;
			}
			painter->setPen(original_pen);
			painter->setFont(original_font);
			
			QIcon item_icon = index.data(Qt::DecorationRole).value<QIcon>();
			item_icon.paint(painter,
							option.rect.left() + m_horizontal_padding,
							option.rect.top() + m_vertical_padding,
							option.decorationSize.width(),
							option.decorationSize.height(),
							Qt::AlignTop);
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
			QRect rect = option.rect;
			rect.setLeft(rect.left() + 2*m_horizontal_padding + option.decorationSize.width());
			QFontMetrics metrics = option.fontMetrics;
			int height = metrics.boundingRect(rect, Qt::TextSingleLine,
											  index.data(Qt::DisplayRole).toString()).height();

			Status status = index.data(ItemStatusRole).value<Status>();

			ItemType type = static_cast<ItemType>(index.data(ItemDataType).toInt());

			bool isContact = (type == ContactType);

			if (isContact && (m_show_flags & ShowStatusText) && !status.text().isEmpty()) {
				QFont desc_font = option.font;
				desc_font.setPointSize(desc_font.pointSize() -1);
				metrics = QFontMetrics(desc_font);
				height += metrics.boundingRect(rect,
											   Qt::TextSingleLine,
											   status.text().remove("\n")
											   ).height();
			}
			height = qMax(option.decorationSize.height(),height);
			height += 2*m_vertical_padding;
			QSize size (option.rect.width(),height);
			return size;
		}

	}
}

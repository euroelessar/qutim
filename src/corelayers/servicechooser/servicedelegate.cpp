/****************************************************************************
 *  servicedelegate.cpp
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

#include "servicedelegate.h"
#include <QApplication>
#include <QPainter>
#include <QMouseEvent>

namespace Core
{
	
	ServiceDelegate::ServiceDelegate(QObject* parent):
		QAbstractItemDelegate(parent),
		m_vertical_padding(6),
		m_horizontal_padding(9)
	{

	}

	void ServiceDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		QStyleOptionViewItemV4 opt(option);
		QStyle *style = opt.widget ? opt.widget->style() : QApplication::style();
		style->drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, painter, opt.widget);

		QIcon item_icon = qvariant_cast<QIcon>(index.data(Qt::DecorationRole));
		
		QIcon::Mode mode = QIcon::Normal;
		
		Qt::CheckState state = static_cast<Qt::CheckState>(index.data(Qt::CheckStateRole).toInt());
		
		QFont painter_font = painter->font();
		
		bool group = index.data(Qt::UserRole).toBool();
		if (!group)
			mode = (state == Qt::Checked) ? QIcon::Normal : QIcon::Disabled;
		else {
			QFont font = painter->font();
			font.setBold(true);
			painter->setFont(font);
		}

		item_icon.paint(painter,
						option.rect,
 						Qt::AlignVCenter,
						mode);

		QString name = index.data(Qt::DisplayRole).toString();

		QRect rect = option.rect;
		rect.setLeft(rect.left() + option.decorationSize.width() + m_horizontal_padding);
		painter->drawText(rect,
							Qt::AlignVCenter,
							name
						);

		painter->setFont(painter_font);

		
	}

	QSize ServiceDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		Q_UNUSED(index);
		QSize size(option.decorationSize);
		size += QSize(0,m_vertical_padding);
		return size;
	}
	
	
	bool ServiceDelegate::editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index)
	{
	    if (event->type() == QEvent::MouseButtonPress) {
			Qt::CheckState state = static_cast<Qt::CheckState>(index.data(Qt::CheckStateRole).toInt());
			bool group = index.data(Qt::UserRole).toBool();			
			if (!group) {
				
				if (state == Qt::Checked)
					state = Qt::Unchecked;
				else
					state = Qt::Checked;
				
				//return false;
				model->setData(index, state , Qt::CheckStateRole);
			}
        }
		return QAbstractItemDelegate::editorEvent(event, model, option, index);
	}



}

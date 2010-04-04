/****************************************************************************
 *  jmucjoinbookmarksitemdelegate.cpp
 *
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

#include <qutim/debug.h>
#include "jmucjoinbookmarksitemdelegate.h"
#include <QPainter>
#include <QApplication>
#include "jbookmarkmanager.h"
#include <qtextlayout.h>

namespace Jabber
{
	QString description(const QModelIndex& index)
	{
		JBookmark bookmark = index.data(Qt::UserRole+1).value<JBookmark>();
		return QString(bookmark.conference + "\n" + bookmark.nick);
	};
	
	JMUCJoinBookmarksItemDelegate::JMUCJoinBookmarksItemDelegate(QObject* parent): 
		QAbstractItemDelegate(parent)
	{

	}
	
	JMUCJoinBookmarksItemDelegate::~JMUCJoinBookmarksItemDelegate()
	{

	}

	void JMUCJoinBookmarksItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		QStyleOptionViewItemV4 opt(option);
		QStyle *style = opt.widget ? opt.widget->style() : QApplication::style();
		style->drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, painter, opt.widget);
		
		QString title = index.data(Qt::DisplayRole).toString();
		QRect rect = option.rect;
		
		const QFont painter_font = painter->font();
		const QPen painter_pen = painter->pen();
	
		QFont title_font = painter_font;
		title_font.setBold(true);
		painter->setFont(title_font);
		painter->drawText(rect,
						  Qt::AlignTop | Qt::AlignLeft,
						  title
						);
						
		QString desc = description(index);
		QFont description_font = painter_font;
		description_font.setPointSize(painter_font.pointSize() -1);
		painter->setFont(description_font);
		painter->setPen(opt.palette.color(QPalette::Shadow));
		
		painter->drawText(rect,
						  Qt::AlignBottom | Qt::AlignLeft | Qt::TextWordWrap,
						  desc);
						  
		painter->setPen(painter_pen);
		painter->setFont(painter_font);
	}

	QSize JMUCJoinBookmarksItemDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		int width = option.rect.width();
		QFontMetrics metrics = option.fontMetrics;
		int height = metrics.boundingRect(option.rect,
										  Qt::TextWordWrap,
										  index.data(Qt::DisplayRole).toString()
										  ).height();
		
		QFont desc_font = option.font;
		desc_font.setPointSize(desc_font.pointSize() -1);
		metrics = QFontMetrics(desc_font);
										  
		height += metrics.boundingRect(option.rect,
									   Qt::TextWordWrap,
									   description(index)
									   ).height();
		QSize size (width,height);
		return size;
	}

}
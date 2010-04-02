/****************************************************************************
 *  servicedelegate.h
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

#ifndef SERVICEDELEGATE_H
#define SERVICEDELEGATE_H

#include <QAbstractItemDelegate>

namespace Core
{

	class ServiceDelegate : public QAbstractItemDelegate
	{
		Q_OBJECT
	public:
		ServiceDelegate(QObject* parent = 0);
		virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
		virtual QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;
	private:
		virtual bool editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index);
		int m_vertical_padding;
		int m_horizontal_padding;
	};

}
#endif // SERVICEDELEGATE_H

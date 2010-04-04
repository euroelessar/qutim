/****************************************************************************
 *  jmucjoinbookmarksitemdelegate.h
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
#ifndef JMUCJOINBOOKMARKSITEMDELEGATE_H
#define JMUCJOINBOOKMARKSITEMDELEGATE_H

#include <QAbstractItemDelegate>

namespace Jabber
{
	class JMUCJoinBookmarksItemDelegate : public QAbstractItemDelegate
	{
		Q_OBJECT
	public:
		JMUCJoinBookmarksItemDelegate(QObject* parent = 0);
		virtual ~JMUCJoinBookmarksItemDelegate();
		virtual QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;
		virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
	private:
		int m_padding;
	};

}
#endif // JMUCJOINBOOKMARKSITEMDELEGATE_H

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
#ifndef ITEMDELEGATE_H
#define ITEMDELEGATE_H

#include <QAbstractItemDelegate>

namespace Core
{
	enum ItemRole
	{
		DescriptionRole = Qt::UserRole + 1,
		SeparatorRole,
		TitleRole, //NOTE: in title and separator description and decoration roles will be ignored
		ActionRole //TODO
	};

	Q_DECLARE_FLAGS(ItemRoles,ItemRole);

	class ItemDelegate : public QAbstractItemDelegate
	{
		Q_OBJECT
	public:
		ItemDelegate(QObject* parent = 0);
		virtual ~ItemDelegate();
		virtual QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;
		virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
		void setCommandLinkStyle(bool style = true);
	private:
		int m_padding;
		bool m_command_link_style;
	};

}

Q_DECLARE_OPERATORS_FOR_FLAGS(Core::ItemRoles);

#endif // ITEMDELEGATE_H

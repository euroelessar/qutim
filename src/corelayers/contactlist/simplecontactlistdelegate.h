/****************************************************************************
 *  simplecontactlistitem.h
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

#ifndef SIMPLECONTACTLISTDELEGATE_H
#define SIMPLECONTACTLISTDELEGATE_H

#include <QAbstractItemDelegate>
#include <QTreeView>

namespace Core
{
	namespace SimpleContactList
	{
		class Delegate : public QAbstractItemDelegate
		{
			Q_OBJECT
		public:
			enum ShowFlags
			{
				ShowStatusText = 0x1,
				ShowExtendedStatusIcons = 0x2,
				ShowClientIcon = 0x4,
				ShowAvatars = 0x8
			};
			explicit Delegate(QTreeView *parent = 0);
			virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
			virtual QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;
			void setShowFlags(ShowFlags flags);
		public Q_SLOTS:
			bool helpEvent(QHelpEvent *event,
						   QAbstractItemView *view,
						   const QStyleOptionViewItem &option,
						   const QModelIndex &index);
		private:
			int m_vertical_padding;
			int m_horizontal_padding;
			ShowFlags m_show_flags;
		};
	}
}

#endif // SIMPLECONTACTLISTDELEGATE_H

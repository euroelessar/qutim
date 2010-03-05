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

#include <QStyledItemDelegate>

namespace Core
{
	namespace SimpleContactList
	{
		class SimpleContactListDelegate : public QStyledItemDelegate
		{
			Q_OBJECT
		public:
			explicit SimpleContactListDelegate(QObject *parent = 0);
			virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
			virtual QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;
		public Q_SLOTS:
			bool helpEvent(QHelpEvent *event,
						   QAbstractItemView *view,
						   const QStyleOptionViewItem &option,
						   const QModelIndex &index);
		private:
		};
	}
}

#endif // SIMPLECONTACTLISTDELEGATE_H

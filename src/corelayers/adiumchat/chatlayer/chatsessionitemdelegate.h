/****************************************************************************
 *  chatsessionitemdelegate.h
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

#ifndef CHATSESSIONITEMDELEGATE_H
#define CHATSESSIONITEMDELEGATE_H

#include <QStyledItemDelegate>

namespace Core
{
	namespace AdiumChat
	{
		class ChatSessionItemDelegate : public QStyledItemDelegate
		{
			Q_OBJECT
		public:
			explicit ChatSessionItemDelegate(QObject *parent = 0);

		public Q_SLOTS:
			bool helpEvent(QHelpEvent *event, QAbstractItemView *view,
						   const QStyleOptionViewItem &option,
						   const QModelIndex &index);
		};
	}
}
#endif // CHATSESSIONITEMDELEGATE_H

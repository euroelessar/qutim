/****************************************************************************
 *  chatsessionitemdelegate.cpp
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

#include "chatsessionitemdelegate.h"
#include "libqutim/tooltip.h"
#include "libqutim/buddy.h"
#include <QHelpEvent>
#include <QAbstractItemView>

using namespace qutim_sdk_0_3;

namespace Core
{
	namespace AdiumChat
	{
		ChatSessionItemDelegate::ChatSessionItemDelegate(QObject *parent) :
				QStyledItemDelegate(parent)
		{
		}

		bool ChatSessionItemDelegate::helpEvent(QHelpEvent *event, QAbstractItemView *view,
												const QStyleOptionViewItem &option,
												const QModelIndex &index)
		{
			if (event->type() == QEvent::ToolTip) {
				Buddy *buddy = index.data(Qt::UserRole).value<Buddy*>();
				ToolTip::instance()->showText(event->globalPos(), buddy, view);
				return true;
			} else {
				return QAbstractItemDelegate::helpEvent(event, view, option, index);
			}
		}
	}
}

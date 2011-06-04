/****************************************************************************
 *
 *  This file is part of qutIM
 *
 *  Copyright (c) 2011 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This file is part of free software; you can redistribute it and/or    *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************
 ****************************************************************************/

#include "packagedelegate.h"
#include "packageentry.h"
#include <qutim/debug.h>

using namespace qutim_sdk_0_3;

// From KNewStuff
enum {
	PreviewWidth  = 96,
	PreviewHeight = 72
};

PackageDelegate::PackageDelegate(QObject *parent) :
    QAbstractItemDelegate(parent)
{
}

void PackageDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                            const QModelIndex &index) const
{
	Q_UNUSED(painter);
	Q_UNUSED(option);
	Q_UNUSED(index);
//	PackageEntry entry = index.data(Qt::UserRole).value<PackageEntry>();
}

QSize PackageDelegate::sizeHint(const QStyleOptionViewItem &option,
                                const QModelIndex &index) const
{
    Q_UNUSED(index);

    QSize size;
    size.setWidth(option.fontMetrics.height() * 4);
	// up to 6 lines of text, and two margins
    size.setHeight(qMax(option.fontMetrics.height() * 7, int(PreviewHeight)));
    return size;

}

void PackageDelegate::updateEditorGeometry(QWidget *editor,
                                           const QStyleOptionViewItem &option,
                                           const QModelIndex &index) const
{
	Q_UNUSED(index);
	editor->setGeometry(option.rect);
}

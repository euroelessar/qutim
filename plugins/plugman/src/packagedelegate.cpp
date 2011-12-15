/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
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


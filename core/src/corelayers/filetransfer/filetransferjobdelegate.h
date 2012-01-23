/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Alexey Prokhin <alexey.prokhin@yandex.ru>
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

#ifndef FILETRANSFERJOBDELEGATE_H
#define FILETRANSFERJOBDELEGATE_H

#include "itemdelegate.h"

using namespace qutim_sdk_0_3;
namespace Core {

class FileTransferJobDelegate : public ItemDelegate
{
public:
	FileTransferJobDelegate(QObject* parent = 0);
	virtual QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;
	virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
};

} // namespace Core

#endif // FILETRANSFERJOBDELEGATE_H


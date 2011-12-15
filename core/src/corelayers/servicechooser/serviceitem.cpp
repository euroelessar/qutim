/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Sidorov Aleksey <sauron@citadelspb.com>
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

#include "serviceitem.h"
#include <qutim/icon.h>

namespace Core
{
	ServiceItem::ServiceItem(const QIcon& icon, const QString& text)
	{
		setText(text);
		setIcon(icon);
	}

	void ServiceItem::setData(const QVariant& value, int role)
	{
		if (role == Qt::CheckStateRole && parent() && parent()->data(ExclusiveRole).toBool()) {
			Qt::CheckState state = static_cast<Qt::CheckState>(value.toInt());
			
			if (state == Qt::Checked) {
				for (int row = 0;row!=parent()->rowCount();row++) {
					parent()->child(row)->setData(Qt::Unchecked,role);
				}
			}
			//TODO provide a situation where none of the items will not be selected
// 			else {
// 				Qt::CheckState current_state = static_cast<Qt::CheckState>(data(Qt::CheckStateRole).toInt());
// 				if (state == Qt::Unchecked && current_state == Qt::Checked) {
// 					return;
// 				}
// 			}
		}
		
		QStandardItem::setData(value, role);
	}

}


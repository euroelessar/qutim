/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2012 Nicolay Izoderov <nico-izo@ya.ru>
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

#include "simplefilterproxymodel.h"
#include "serviceitem.h"
#include <QSortFilterProxyModel>

namespace Core
{
	SimpleFilterProxyModel::SimpleFilterProxyModel(QObject *parent)
		: QSortFilterProxyModel(parent)
	{
	}

	bool SimpleFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
	{
		QModelIndex index0 = sourceModel()->index(sourceRow, 0, sourceParent);
		if(!m_complexHandling)
			return (sourceModel()->data(index0, qutim_sdk_0_3::DescriptionRole).toString().contains(filterRegExp())
					|| sourceModel()->data(index0).toString().contains(filterRegExp()));

		if(index0.child(0, 0).isValid()) {
			for(int i = 0; index0.child(i, 0).isValid(); ++i) {
				if(sourceModel()->data(index0.child(i, 0)).toString().contains(filterRegExp())
				   || sourceModel()->data(index0.child(i, 0), qutim_sdk_0_3::DescriptionRole).toString().contains(filterRegExp()))
					return true;
			}
		} else {
			return (sourceModel()->data(index0, qutim_sdk_0_3::DescriptionRole).toString().contains(filterRegExp())
					|| sourceModel()->data(index0).toString().contains(filterRegExp()));
		}

		return false;

	}

	void SimpleFilterProxyModel::setComplexHandling(bool handle)
	{
		m_complexHandling = handle;
	}
}


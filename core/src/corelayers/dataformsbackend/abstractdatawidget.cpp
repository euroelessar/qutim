/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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
#include "abstractdatawidget.h"

namespace Core {

AbstractDataWidget::AbstractDataWidget(const qutim_sdk_0_3::DataItem &item, DefaultDataForm *dataForm) :
	m_dataForm(dataForm)
{
	m_item = item;
	if (!m_item.isNull()) {
		// We need only item properties, right?
		m_item.setSubitems(QList<qutim_sdk_0_3::DataItem>());
	}
}

AbstractDataWidget::~AbstractDataWidget()
{
}

void AbstractDataWidget::setData(const QVariant &data)
{
	Q_UNUSED(data);
}

} // namespace Core


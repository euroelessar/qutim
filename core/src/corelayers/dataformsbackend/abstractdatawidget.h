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
#ifndef ABSTRACTDATAWIDGET_H
#define ABSTRACTDATAWIDGET_H

#include <qutim/dataforms.h>

namespace Core {

class DefaultDataForm;

class AbstractDataWidget
{
public:
	AbstractDataWidget(const qutim_sdk_0_3::DataItem &item, DefaultDataForm *dataForm);
	virtual ~AbstractDataWidget();
	virtual qutim_sdk_0_3::DataItem item() const = 0;
	virtual void setData(const QVariant &data);
	DefaultDataForm *dataForm() { return m_dataForm; }
protected:
	qutim_sdk_0_3::DataItem m_item;
private:
	DefaultDataForm *m_dataForm;
};

} // namespace Core

Q_DECLARE_INTERFACE(Core::AbstractDataWidget, "org.qutim.core.AbstractDataWidget");

#endif // ABSTRACTDATAWIDGET_H


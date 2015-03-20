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
#ifndef DATAFORMSBACKEND_H
#define DATAFORMSBACKEND_H

#include <qutim/dataforms.h>
#include "abstractdatawidget.h"

class QAbstractButton;
class QDialogButtonBox;

namespace Core
{

using namespace qutim_sdk_0_3;

class DefaultDataForm : public AbstractDataForm
{
	Q_OBJECT
public:
	DefaultDataForm(const DataItem &item);
	virtual DataItem item() const;
	virtual bool isChanged() const;
	virtual bool isComplete() const;
	virtual void clearState();
	virtual void setData(const QString &name, const QVariant &data);
	inline void addWidget(const QString &name, AbstractDataWidget *widget)
	{
		m_widgets.insert(name, widget);
	}
public slots:
	void dataChanged();
	void completeChanged(bool complete);
private:
	AbstractDataWidget *m_widget;
	bool m_isChanged;
	int m_incompleteWidgets;
	QMultiHash<QString, AbstractDataWidget*> m_widgets;
	QDialogButtonBox *m_buttonsBox;
	bool m_hasSubitems;
};

class DefaultDataFormsBackend : public DataFormsBackend
{
	Q_OBJECT
	Q_CLASSINFO("Service", "DataFormsBackend")
public:
	DefaultDataFormsBackend();
	virtual AbstractDataForm *get(const DataItem &item);
};

}

#endif // DATAFORMSBACKEND_H


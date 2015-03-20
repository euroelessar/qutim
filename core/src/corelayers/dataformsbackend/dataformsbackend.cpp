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
#include "dataformsbackend.h"
#include "modifiablewidget.h"
#include "datalayout.h"
#include "widgetgenerator.h"
#include <QDialogButtonBox>
#include <QPushButton>
#include <QKeyEvent>

#include <qutim/icon.h>

Q_DECLARE_METATYPE(QList<QIcon>)

namespace Core
{

DefaultDataForm::DefaultDataForm(const DataItem &item) :
	m_widget(0),
	m_isChanged(false),
	m_incompleteWidgets(0),
	m_buttonsBox(0),
	m_hasSubitems(item.hasSubitems() || item.isAllowedModifySubitems())
{
	DataLayout *dataLayout = 0;
	QVBoxLayout *layout = 0;
	setObjectName(item.name());
	setWindowTitle(item.title());
	if (item.isAllowedModifySubitems()) {
		layout = new QVBoxLayout(this);
		ModifiableWidget *w = new ModifiableWidget(item, this, this);
		m_widget = w;
		layout->addWidget(w);
		if (!w->isExpandable()) {
			QSpacerItem *spacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
			layout->addItem(spacer);
		}
	} else {
		dataLayout = new DataLayout(item, this, item.property<quint16>("columns", 1), this);
		QVariant spacing = item.property("horizontalSpacing");
		if (spacing.canConvert(QVariant::Int))
			dataLayout->setHorizontalSpacing(spacing.toInt());
		spacing = item.property("verticalSpacing");
		if (spacing.canConvert(QVariant::Int))
			dataLayout->setVerticalSpacing(spacing.toInt());

		m_widget = dataLayout;
		if (item.hasSubitems())
			dataLayout->addDataItems(item.subitems());
		else
			dataLayout->addDataItem(item);
		if (!dataLayout->isExpandable())
			dataLayout->addSpacer();
	}
}

DataItem DefaultDataForm::item() const
{
	if (m_hasSubitems) {
		DataItem item;
		if (m_widget)
			item = m_widget->item();
		item.setName(objectName());
		item.setTitle(windowTitle());
		return item;
	} else {
		return static_cast<DataLayout*>(m_widget)->item(false);
	}
}

bool DefaultDataForm::isChanged() const
{
	return m_isChanged;
}

bool DefaultDataForm::isComplete() const
{
	return m_incompleteWidgets == 0;
}

void DefaultDataForm::clearState()
{
	m_isChanged = false;
}

void DefaultDataForm::setData(const QString &name, const QVariant &data)
{
	foreach (AbstractDataWidget *widget, m_widgets.values(name))
		widget->setData(data);
}

void DefaultDataForm::dataChanged()
{
	if (!m_isChanged) {
		emit changed();
		m_isChanged = true;
	}
}

void DefaultDataForm::completeChanged(bool complete)
{
	if (complete) {
		--m_incompleteWidgets;
		Q_ASSERT(m_incompleteWidgets >= 0);
		if (m_incompleteWidgets == 0)
			emit AbstractDataForm::completeChanged(true);
	} else {
		if (m_incompleteWidgets == 0)
			emit AbstractDataForm::completeChanged(false);
		++m_incompleteWidgets;
	}
}

DefaultDataFormsBackend::DefaultDataFormsBackend()
{
}

AbstractDataForm *DefaultDataFormsBackend::get(const DataItem &item)
{
	if (item.isNull())
		return 0;
	return new DefaultDataForm(item);
}

}


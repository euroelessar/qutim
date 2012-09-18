/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2012 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#include "xsettingswidget.h"
#include <QScrollArea>

namespace Core {

using namespace qutim_sdk_0_3;

XSettingsWidget::XSettingsWidget(QWidget *parent) :
    SettingsWidget(parent)
{
	QVBoxLayout *mainLayout = new QVBoxLayout(this);
	QScrollArea *area = new QScrollArea(this);
	QWidget *mainWidget = new QWidget(area);
	area->setWidget(mainWidget);
	area->setWidgetResizable(true);
	area->setFrameStyle(QFrame::NoFrame);
	mainLayout->setContentsMargins(0, 0, 0, 0);
	mainLayout->addWidget(area);
	m_layout = new QVBoxLayout(mainWidget);
	m_layout->setContentsMargins(0, 0, 0, 0);
}

void XSettingsWidget::setController(QObject *controller)
{
	m_controller = controller;
	foreach (SettingsWidget *widget, m_widgets)
		widget->setController(controller);
}

static bool itemLessThen(SettingsItem *a, SettingsItem *b)
{
	return a->order() < b->order();
}

void XSettingsWidget::addItem(SettingsItem *item)
{
	if (m_items.contains(item))
		return;
	SettingsWidget *widget = item->widget();
	connect(widget, SIGNAL(modifiedChanged(bool)), SLOT(onModifiedChanged(bool)));
	SettingsItemList::Iterator it = qLowerBound(m_items.begin(), m_items.end(),
	                                            item, itemLessThen);
	int index = it - m_items.begin();
	m_items.insert(index, item);
	m_widgets.insert(index, widget);
	m_layout->insertWidget(index, widget);
	if (m_layout->count() == 2)
		m_layout->addStretch();
}

bool XSettingsWidget::removeItem(SettingsItem *item)
{
	int index = m_items.indexOf(item);
	if (index == -1)
		return false;
	m_items.removeAt(index);
	SettingsWidget *widget = m_widgets.takeAt(index);
	m_layout->removeWidget(widget);
	m_changed.remove(widget);
	item->clearWidget();
	if (m_layout->count() == 2)
		delete m_layout->takeAt(1);
	return true;
}

void XSettingsWidget::loadImpl()
{
	foreach (SettingsWidget *widget, m_widgets)
		widget->load();
}

void XSettingsWidget::saveImpl()
{
	foreach (SettingsWidget *widget, m_widgets)
		widget->save();
}

void XSettingsWidget::cancelImpl()
{
	foreach (SettingsWidget *widget, m_widgets)
		widget->cancel();
}

void XSettingsWidget::onModifiedChanged(bool modified)
{
	if (modified)
		m_changed.insert(sender());
	else
		m_changed.remove(sender());
	setModified(!m_changed.isEmpty());
}

} // namespace Core

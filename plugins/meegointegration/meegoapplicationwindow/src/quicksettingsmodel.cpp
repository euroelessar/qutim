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

#include "quicksettingsmodel.h"
#include <qutim/settingswidget.h>

namespace MeegoIntegration
{
using namespace qutim_sdk_0_3;

QuickSettingsModel::QuickSettingsModel(QObject *parent) :
    QAbstractListModel(parent)
{
	QHash<int,QByteArray> roles = roleNames();
	roles[IsWidget] = "isWidget";
	roles[Widget] = "widget";
	roles[GraphicsItem] = "graphicsItem";
	setRoleNames(roles);
}

bool itemLessThen(SettingsItem *a, SettingsItem *b)
{
	return a->text().toString() < b->text().toString();
}

void QuickSettingsModel::setItems(const qutim_sdk_0_3::SettingsItemList &items, QObject *controller)
{
	m_controller = controller;
	if (!m_items.isEmpty()) {
		beginRemoveRows(QModelIndex(), 0, m_items.size());
		m_items.clear();
		endRemoveRows();
	}
	beginInsertRows(QModelIndex(), 0, items.size());
	m_items = items;
	qSort(m_items.begin(), m_items.end(), itemLessThen);
	endInsertRows();
}

QObject *QuickSettingsModel::widget(int index)
{
	SettingsItem *item = m_items.value(index);
	if (item) {
		SettingsWidget *widget = item->widget();
		widget->setController(m_controller);
		return widget;
	}
	return NULL;
}

int QuickSettingsModel::rowCount(const QModelIndex &parent) const
{
	return m_items.size();
}

QVariant QuickSettingsModel::data(const QModelIndex &index, int role) const
{
	SettingsItem *item = m_items.value(index.row());
	if (!item)
		return QVariant();
	switch (role) {
	case Qt::DecorationRole:
		return QVariant();
	case Qt::DisplayRole:
		return item->text().toString();
	case IsWidget:
		return true;
	case Widget:
		return qVariantFromValue<QObject*>(item->widget());
	case GraphicsItem:
		return qVariantFromValue<QObject*>(NULL);
	default:
		return QVariant();
	}
}
}


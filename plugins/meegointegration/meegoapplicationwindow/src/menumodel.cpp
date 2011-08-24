/****************************************************************************
**
** qutIM instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@ya.ru>
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

#include "menumodel.h"
#include <qdeclarative.h>

namespace MeegoIntegration
{
using namespace qutim_sdk_0_3;

MenuModel::MenuModel()
    : m_controller(0)
{
	QHash<int, QByteArray> roleNames;
	roleNames.insert(Qt::UserRole, "action");
	setRoleNames(roleNames);
	m_container.addHandler(this);
	m_container.show();
}

MenuModel::~MenuModel()
{
}

void MenuModel::init()
{
	qmlRegisterType<MenuModel>("org.qutim", 0, 3, "MenuModel");
//	qmlRegisterType<MenuController>();
}

QObject *MenuModel::controller() const
{
	return m_controller;
}

void MenuModel::setController(QObject *object)
{
	qutim_sdk_0_3::MenuController *controller = qobject_cast<MenuController*>(object);
	if (m_controller == controller)
		return;
	m_controller = controller;
	m_container.setController(controller);
	emit controllerChanged(controller);
}

void MenuModel::actionAdded(QAction *action, int index)
{
	beginInsertRows(QModelIndex(), index, index);
	m_actions.insert(index, action);
	endInsertRows();
}

void MenuModel::actionRemoved(int index)
{
	beginRemoveRows(QModelIndex(), index, index);
	m_actions.removeAt(index);
	endRemoveRows();
}

void MenuModel::actionsCleared()
{
	beginResetModel();
	m_actions.clear();
	endResetModel();
}

int MenuModel::rowCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	return m_actions.size();
}

QVariant MenuModel::data(const QModelIndex &index, int role) const
{
	if (role != Qt::UserRole)
		return QVariant();
	return qVariantFromValue(m_actions.value(index.row()));
}
}

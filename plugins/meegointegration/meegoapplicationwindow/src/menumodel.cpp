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
#include <QDebug>

namespace MeegoIntegration
{
using namespace qutim_sdk_0_3;

MenuModel::MenuModel()
    : m_controller(0)
{
	qDebug("%s %d", Q_FUNC_INFO, __LINE__);
	QHash<int, QByteArray> roleNames;
	qDebug("%s %d", Q_FUNC_INFO, __LINE__);
	roleNames.insert(Qt::UserRole, "action");
	qDebug("%s %d", Q_FUNC_INFO, __LINE__);
	setRoleNames(roleNames);
	qDebug("%s %d", Q_FUNC_INFO, __LINE__);
	m_container.addHandler(this);
	m_container.show();
}

MenuModel::~MenuModel()
{
	qDebug("%s %d", Q_FUNC_INFO, __LINE__);
}

void MenuModel::init()
{
	qDebug("%s %d", Q_FUNC_INFO, __LINE__);
	qmlRegisterType<MenuModel>("org.qutim", 0, 3, "MenuModel");
//	qmlRegisterType<MenuController>();
	qDebug("%s %d", Q_FUNC_INFO, __LINE__);
}

QObject *MenuModel::controller() const
{
	qDebug("%s %d", Q_FUNC_INFO, __LINE__);
	return m_controller;
}

void MenuModel::setController(QObject *object)
{
	qutim_sdk_0_3::MenuController *controller = qobject_cast<MenuController*>(object);
	qDebug() << Q_FUNC_INFO << __LINE__ << object;
	if (m_controller == controller)
		return;
	m_controller = controller;
	qDebug("%s %d", Q_FUNC_INFO, __LINE__);
	m_container.setController(controller);
	emit controllerChanged(controller);
	qDebug("%s %d", Q_FUNC_INFO, __LINE__);
}

void MenuModel::actionAdded(QAction *action, int index)
{
	qDebug("%s %d", Q_FUNC_INFO, __LINE__);
	beginInsertRows(QModelIndex(), index, index);
	qDebug("%s %d", Q_FUNC_INFO, __LINE__);
	m_actions.insert(index, action);
	qDebug("%s %d", Q_FUNC_INFO, __LINE__);
	endInsertRows();
	qDebug("%s %d", Q_FUNC_INFO, __LINE__);
}

void MenuModel::actionRemoved(int index)
{
	qDebug("%s %d", Q_FUNC_INFO, __LINE__);
	beginRemoveRows(QModelIndex(), index, index);
	qDebug("%s %d", Q_FUNC_INFO, __LINE__);
	m_actions.removeAt(index);
	qDebug("%s %d", Q_FUNC_INFO, __LINE__);
	endRemoveRows();
	qDebug("%s %d", Q_FUNC_INFO, __LINE__);
}

void MenuModel::actionsCleared()
{
	qDebug("%s %d", Q_FUNC_INFO, __LINE__);
	beginRemoveRows(QModelIndex(), 0, m_actions.size() - 1);
	qDebug("%s %d", Q_FUNC_INFO, __LINE__);
	m_actions.clear();
	qDebug("%s %d", Q_FUNC_INFO, __LINE__);
	endRemoveRows();
	qDebug("%s %d", Q_FUNC_INFO, __LINE__);
}

int MenuModel::rowCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	qDebug("%s %d %d", Q_FUNC_INFO, __LINE__, m_actions.size());
	return m_actions.size();
}

QVariant MenuModel::data(const QModelIndex &index, int role) const
{
	qDebug("%s %d", Q_FUNC_INFO, __LINE__);
	if (role != Qt::UserRole)
		return QVariant();
	qDebug("%s %d", Q_FUNC_INFO, __LINE__);
	return qVariantFromValue(m_actions.value(index.row()));
}
}

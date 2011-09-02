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
#include <QCoreApplication>

namespace MeegoIntegration
{
using namespace qutim_sdk_0_3;

enum {
	HasChildrenRole = -1,
	ActionRole = Qt::UserRole,
	ModelDataRole
};

MenuModel::MenuModel()
    : m_controller(0)
{
	QHash<int, QByteArray> roleNames;
	roleNames.insert(ActionRole, "action");
	roleNames.insert(ModelDataRole, "modelData");
	roleNames.insert(HasChildrenRole, "hasModelChildren");
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
	Node *parent = ensureNode(m_container.menu(index));
	qDebug() << Q_FUNC_INFO << action->text() << parent->name;
	Node::Ptr prev = m_actions.value(index - 1);
	int row = 0;
	if (prev && prev->parent == parent)
		row = parent->children.indexOf(prev) + 1;
	QModelIndex parentIndex = createNodeIndex(parent);
	beginInsertRows(parentIndex, row, row);
	Node::Ptr node = Node::Ptr(new Node(action, parent));
	node->name = m_container.generator(index)->text().original();
	m_actions.insert(index, node);
	parent->children.insert(row, node);
	endInsertRows();
	emit dataChanged(parentIndex, parentIndex);
}

void MenuModel::actionRemoved(int index)
{
	Node::Ptr node = m_actions.takeAt(index);
	int row = node->parent->children.indexOf(node);
	Q_ASSERT(row >= 0);
	QModelIndex parentIndex = createNodeIndex(node->parent);
	beginRemoveRows(parentIndex, row, row);
	node->parent->children.removeAt(row);
	delete node;
	endRemoveRows();
}

void MenuModel::actionsCleared()
{
	beginResetModel();
	qDeleteAll(m_root.children);
	m_root.children.clear();
	m_actions.clear();
	endResetModel();
}

QModelIndex MenuModel::index(int row, int column, const QModelIndex &parent) const
{
	Node *parentNode = nodeCast(parent);
	Q_ASSERT(parentNode);
	Node *node = parentNode->children[row];
	return createIndex(row, column, node);
}

QModelIndex MenuModel::parent(const QModelIndex &child) const
{
	Node *node = nodeCast(child);
	Q_ASSERT(node);
	return createNodeIndex(node->parent);
}

int MenuModel::columnCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	return 1;
}

bool MenuModel::hasChildren(const QModelIndex &parent) const
{
	Node *node = nodeCast(parent);
	Q_ASSERT(node);
	qDebug() << Q_FUNC_INFO << node->name << !node->children.isEmpty();
	return node && !node->children.isEmpty();
}

int MenuModel::rowCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	return m_actions.size();
}

QVariant MenuModel::data(const QModelIndex &index, int role) const
{
	Node *node = nodeCast(index);
	Q_ASSERT(node);
	Q_ASSERT(node->action);
	qDebug() << Q_FUNC_INFO << node->name << node->action;
	switch (role) {
	case HasChildrenRole:
		return !node->children.isEmpty();
	case ActionRole:
	case ModelDataRole:
		return qVariantFromValue(node->action);
	default:
		return QVariant();
	}

	return (node && node->action) ? qVariantFromValue(node->action) : QVariant();
}

QModelIndex MenuModel::createNodeIndex(Node *node) const
{
	if (!node || node == &m_root)
		return QModelIndex();
	Q_ASSERT(node->parent);
	int row = node->parent->children.indexOf(node);
	Q_ASSERT(row >= 0);
	return createIndex(row, 0, node);
}

MenuModel::Node *MenuModel::ensureNode(const QList<QByteArray> &menu)
{
	Node *node = &m_root;
	Node *child = 0;
	bool found = false;
	for (int i = 0; i < menu.size(); ++i) {
		found = false;
		for (int j = 0; j < node->children.size(); ++j) {
			child = node->children[j];
			if (child->name == menu[i]) {
				found = true;
				break;
			}
		}
		if (!found) {
			QString name = QCoreApplication::translate("Menu", menu[i]);
			QAction *action = new QAction(name, this);
			int row = node->children.size();
			beginInsertRows(createNodeIndex(node), row, row);
			child = new Node(action, node);
			child->name = menu[i];
			child->deleteAction = true;
			node->children.insert(row, child);
			endInsertRows();
		}
		node = child;
	}
	return node;
}

MenuModel::Node *MenuModel::nodeCast(const QModelIndex &index) const
{
	if (!index.isValid())
		return const_cast<Node*>(&m_root);
	return reinterpret_cast<Node*>(index.internalPointer());
}

int MenuModel::Node::counter = 0;
}

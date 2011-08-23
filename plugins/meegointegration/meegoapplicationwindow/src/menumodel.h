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

#ifndef MENUMODEL_H
#define MENUMODEL_H

#include <QAbstractListModel>
#include <qutim/menucontroller.h>
#include <qdeclarative.h>

QML_DECLARE_TYPE_HASMETATYPE(qutim_sdk_0_3::MenuController)

namespace MeegoIntegration
{
class MenuModel : public QAbstractListModel, public qutim_sdk_0_3::ActionHandler
{
    Q_OBJECT
	Q_PROPERTY(QObject* controller READ controller WRITE setController NOTIFY controllerChanged)
public:
    explicit MenuModel();
	virtual ~MenuModel();
	
	static void init();
	
	QObject *controller() const;
	void setController(QObject *controller);
	
	// ActionHandler
	virtual void actionAdded(QAction *action, int index);
	virtual void actionRemoved(int index);
	virtual void actionsCleared();
	
	// QAbstractListModel
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	
signals:
	void controllerChanged(QObject *controller);
	
private:
	qutim_sdk_0_3::MenuController *m_controller;
	qutim_sdk_0_3::ActionContainer m_container;
	QList<QObject*> m_actions;
};
}

#endif // MENUMODEL_H

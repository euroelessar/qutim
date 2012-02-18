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

#ifndef SETTINGSMODEL_H
#define SETTINGSMODEL_H

#include <QAbstractListModel>
#include <qutim/settingslayer.h>

namespace MeegoIntegration
{
class QuickSettingsModel : public QAbstractListModel
{
    Q_OBJECT
public:
	enum { IsWidget = Qt::UserRole, Widget, GraphicsItem };
    explicit QuickSettingsModel(QObject *parent = 0);
	
	void setItems(const qutim_sdk_0_3::SettingsItemList &items, QObject *controller);
	
	Q_INVOKABLE QObject *widget(int index);
	
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	
private:
	QObject *m_controller;
	qutim_sdk_0_3::SettingsItemList m_items;
};
}

#endif // SETTINGSMODEL_H


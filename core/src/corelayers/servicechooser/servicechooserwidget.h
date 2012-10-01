/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Aleksey Sidorov <gorthauer87@yandex.ru>
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

#ifndef SERVICECHOOSERWIDGET_H
#define SERVICECHOOSERWIDGET_H
#include <qutim/settingswidget.h>
#include <QHash>
#include <QModelIndex>
#include "simplefilterproxymodel.h"

namespace qutim_sdk_0_3 
{
class ExtensionInfo;
}

namespace Ui
{
class ServiceChoooser;
}

class QStandardItem;
class QStandardItemModel;
namespace Core
{

class ServiceItem;
using namespace qutim_sdk_0_3;

class ServiceChoooserWidget : public SettingsWidget
{
	Q_OBJECT
public:
	ServiceChoooserWidget();
	virtual void loadImpl();
	virtual void saveImpl();
	virtual void cancelImpl();
	virtual ~ServiceChoooserWidget();
private slots:
	void onItemChanged(QStandardItem*);
	void onItemClicked(QModelIndex index);
	void filterServices(const QString& servicename);
private:
	void clear();
	Ui::ServiceChoooser *ui;
	QStandardItemModel *m_model;
	SimpleFilterProxyModel *m_proxymodel;
	QHash<QByteArray, ServiceItem *> m_service_items;
};

}
#endif // SERVICECHOOSERWIDGET_H


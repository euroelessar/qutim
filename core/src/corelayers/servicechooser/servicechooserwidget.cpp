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

#include "servicechooserwidget.h"
#include "ui_servicechooserwidget.h"
#include <QStandardItem>
#include <QModelIndex>
#include <qutim/extensioninfo.h>
#include <qutim/debug.h>
#include <qutim/icon.h>
#include "qutim/metaobjectbuilder.h"
#include <QStringBuilder>
#include "itemdelegate.h"
#include "serviceitem.h"
#include "servicechooser.h"
#include <qutim/configbase.h>
#include <qutim/notification.h>
#include <qutim/servicemanager.h>

namespace Core
{
ServiceChooserWidget::ServiceChooserWidget() :
	ui(new Ui::ServiceChooser),
	m_model(new QStandardItemModel)
{
	ui->setupUi(this);
	ui->treeView->setModel(m_model);
	ui->treeView->setItemDelegate(new ItemDelegate(ui->treeView));
	ui->treeView->setAnimated(false);
	ui->treeView->setExpandsOnDoubleClick(false);

	connect(ui->treeView,SIGNAL(clicked(QModelIndex)),SLOT(onItemClicked(QModelIndex)));
	connect(m_model,SIGNAL(itemChanged(QStandardItem*)),SLOT(onItemChanged(QStandardItem*)));
}
ServiceChooserWidget::~ServiceChooserWidget()
{
	delete ui;
}

const char *serviceIcon(const char *serviceName)
{
	if (!qstrcmp(serviceName,"TrayIcon"))
		return "user-desktop";
	if (!qstrcmp(serviceName,"ChatLayer"))
		return "view-list-text";
	if (!qstrcmp(serviceName,"ContactList"))
		return "view-list-details";
	return "applications-system";
}

void ServiceChooserWidget::loadImpl()
{
	clear();
	ConfigGroup group = Config().group("services");
	QVariantMap selected = group.value("list", QVariantMap());
	QStandardItem *parent_item = m_model->invisibleRootItem();

	ExtensionInfoList exts = extensionList();
	QStringList helper;
	for (int i = 0; i < exts.size(); i++) {
		const ExtensionInfo &info = exts.at(i);
		const char *serviceName = MetaObjectBuilder::info(info.generator()->metaObject(), "Service");

		if (serviceName && *serviceName) {
			if (!m_service_items.contains(serviceName)) {
				QString localizedName = QT_TRANSLATE_NOOP("Service",serviceName).toString();
				int index = qLowerBound(helper, localizedName) - helper.constBegin();
				helper.insert(index, localizedName);
				ServiceItem *item = new ServiceItem(Icon(serviceIcon(serviceName)),localizedName);
				item->setData(true,ServiceItem::ExclusiveRole);
				parent_item->insertRow(index, item);
				m_service_items.insert(serviceName,item);
			}
			QIcon icon = !info.icon().name().isEmpty() ?
						 info.icon() :
						 Icon("applications-system");
			ServiceItem *item = new ServiceItem(icon,info.name());

			item->setToolTip(ServiceChooser::html(info));
			item->setCheckable(true);
			item->setData(info.description().toString(),DescriptionRole);
			if (selected.value(serviceName).toString() == ServiceChooser::className(info))
				item->setCheckState(Qt::Checked);
			item->setData(qVariantFromValue(info), ServiceItem::ExtentionInfoRole);

			m_service_items.value(serviceName)->appendRow(item);
		}
	}
}
void ServiceChooserWidget::cancelImpl()
{

}
void ServiceChooserWidget::saveImpl()
{
	bool showNotification = false;
	QHash<QByteArray, ServiceItem *>::const_iterator it;
	for (it = m_service_items.constBegin(); it != m_service_items.constEnd(); ++it) {
		ExtensionInfo service;
		for (int i = 0; i != it.value()->rowCount(); ++i) {
			Qt::CheckState state = static_cast<Qt::CheckState>(it.value()->child(i)->data(Qt::CheckStateRole).toInt());
			if (state == Qt::Checked) {
				service = it.value()->child(i)->data(ServiceItem::ExtentionInfoRole).value<ExtensionInfo>();
				break;
			}
		}
		showNotification = !ServiceManager::setImplementation(it.key(), service) || showNotification;
	}
	if (showNotification)
		Notification::send(tr("To take effect you must restart qutIM"));
}

void ServiceChooserWidget::clear()
{
	m_model->clear();
	m_service_items.clear();
}

void ServiceChooserWidget::onItemChanged(QStandardItem* )
{
	emit modifiedChanged(true);
}

void ServiceChooserWidget::onItemClicked(QModelIndex index)
{
	if (ui->treeView->isExpanded(index))
		ui->treeView->collapse(index);
	else
		ui->treeView->expand(index);

#ifdef QUTIM_MOBILE_UI
	QStandardItem *item = m_model->itemFromIndex(index);
	if (item) {
		item->setCheckState(item->checkState() == Qt::Unchecked ? Qt::Checked : Qt::Unchecked);
	}
#endif
}

}


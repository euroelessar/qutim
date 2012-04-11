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

#include "protocolchooserwidget.h"
#include "ui_servicechooserwidget.h"
#include <QStandardItem>
#include <QModelIndex>
#include <qutim/extensioninfo.h>
#include <qutim/debug.h>
#include <qutim/icon.h>
#include "qutim/metaobjectbuilder.h"
#include <QStringBuilder>
#include <qutim/itemdelegate.h>
#include "serviceitem.h"
#include "servicechooser.h"
#include <qutim/configbase.h>
#include <qutim/notification.h>

namespace Core
{
ProtocolChooserWidget::ProtocolChooserWidget() :
	ui(new Ui::ServiceChooser),
	m_model(new QStandardItemModel)
{
	ui->setupUi(this);
	ui->treeView->setModel(m_model);
	ui->treeView->setItemDelegate(new  ItemDelegate(ui->treeView));
	ui->treeView->setAnimated(false);
	ui->treeView->setExpandsOnDoubleClick(false);
	ui->search->hide();
	connect(ui->treeView,SIGNAL(activated(QModelIndex)),SLOT(onItemClicked(QModelIndex)));

	connect(m_model,SIGNAL(itemChanged(QStandardItem*)),SLOT(onItemChanged(QStandardItem*)));
}
ProtocolChooserWidget::~ProtocolChooserWidget()
{
	delete ui;
}

void ProtocolChooserWidget::loadImpl()
{
	clear();
	ConfigGroup group = Config().group("protocols");
	QVariantMap selected = group.value("list", QVariantMap());
	QStandardItem *parent_item = m_model->invisibleRootItem();

	ExtensionInfoList exts = extensionList();
	for (int i = 0; i < exts.size(); i++) {
		const ExtensionInfo &info = exts.at(i);
		const QMetaObject *meta = info.generator()->metaObject();
		QString name = QString::fromLatin1(MetaObjectBuilder::info(meta, "Protocol"));
		if (name.isEmpty())
			continue;

		if (!m_protocol_items.contains(name)) {
			ServiceItem *item = new ServiceItem(Icon("applications-system") ,name);
			item->setData(true,ServiceItem::ExclusiveRole);
			parent_item->appendRow(item);
			m_protocol_items.insert(name,item);
		}
		QIcon icon = Icon("applications-system");
		//TODO Make normal names for the implementation of protocols
		ServiceItem *item = new ServiceItem(icon,info.name());
		//ServiceItem *item = new ServiceItem(icon,info.name());

		item->setToolTip(ServiceChooser::html(info));
		item->setCheckable(true);
		item->setData(info.description().toString(),DescriptionRole);
		if (selected.value(name).toString() == ServiceChooser::className(info))
			item->setCheckState(Qt::Checked);
		item->setData(QLatin1String(ServiceChooser::className(info)),
		              ServiceItem::ExtentionInfoRole);
		m_protocol_items.value(name)->appendRow(item);
	}
}

void ProtocolChooserWidget::cancelImpl()
{

}

void ProtocolChooserWidget::saveImpl()
{
	Config group = Config().group("protocols/list");
	QHash<QString, ServiceItem *>::const_iterator it;
	for (it = m_protocol_items.constBegin();it!=m_protocol_items.constEnd();it++) {
		QVariant service = QLatin1String("none");
		for (int i =0;i!=it.value()->rowCount();i++) {
			Qt::CheckState state = static_cast<Qt::CheckState>(it.value()->child(i)->data(Qt::CheckStateRole).toInt());
			if (state == Qt::Checked) {
				service = it.value()->child(i)->data(ServiceItem::ExtentionInfoRole);
				break;
			}
		}
		group.setValue(it.key(),service);
	}
	Notification::send(tr("To take effect you must restart qutIM"));
}

void ProtocolChooserWidget::clear()
{
	m_model->clear();
	m_protocol_items.clear();
}

void ProtocolChooserWidget::onItemChanged(QStandardItem* )
{
	emit modifiedChanged(true);
}

void ProtocolChooserWidget::onItemClicked(QModelIndex index)
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


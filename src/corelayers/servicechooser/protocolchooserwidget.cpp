/****************************************************************************
 *  protocolchooserwidget.cpp
 *
 *  Copyright (c) 2010 by Aleksey Sidorov <sauron@citadelspb.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#include "protocolchooserwidget.h"
#include "ui_servicechooserwidget.h"
#include <QStandardItem>
#include <libqutim/extensioninfo.h>
#include <libqutim/debug.h>
#include <libqutim/icon.h>
#include <QStringBuilder>
#include "servicedelegate.h"
#include "serviceitem.h"
#include "servicechooser.h"
#include <libqutim/configbase.h>
#include <libqutim/notificationslayer.h>

namespace Core
{
	ProtocolChooserWidget::ProtocolChooserWidget() :
	ui(new Ui::ServiceChooser),
	m_model(new QStandardItemModel)
	{
		ui->setupUi(this);
		ui->toolButton->hide();
		ui->serviceInfo->hide();
		ui->treeView->setModel(m_model);
		ui->treeView->setItemDelegate(new ServiceDelegate(this));
		
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
			QString name = QString::fromLatin1(metaInfo(meta, "Protocol"));
			if (name.isEmpty())
				continue;
			
			if (!m_protocol_items.contains(name)) {
				ServiceItem *item = new ServiceItem(Icon("applications-system") ,name,true);
				item->setData(true,Qt::UserRole);
				parent_item->appendRow(item);
				m_protocol_items.insert(name,item);
			}
			QIcon icon = Icon("help-hint");
			//TODO Make normal names for the implementation of protocols			
			ServiceItem *item = new ServiceItem(icon,info.name());
			//ServiceItem *item = new ServiceItem(icon,info.name());

			item->setToolTip(ServiceChooser::html(info));
			item->setCheckable(true);
			if (selected.value(name).toString() == ServiceChooser::className(info))
				item->setCheckState(Qt::Checked);
			item->setData(false,Qt::UserRole);
			item->setServiceClassName(ServiceChooser::className(info));

			m_protocol_items.value(name)->appendRow(item);
			}
	}
	void ProtocolChooserWidget::cancelImpl()
	{

	}
	void ProtocolChooserWidget::saveImpl()
	{
		ConfigGroup group = Config().group("protocols");
		QVariantMap selected;
		QHash<QString, ServiceItem *>::const_iterator it;
		for (it = m_protocol_items.constBegin();it!=m_protocol_items.constEnd();it++) {
			QVariant service;
			for (int i =0;i!=it.value()->rowCount();i++) {
				Qt::CheckState state = static_cast<Qt::CheckState>(it.value()->child(i)->data(Qt::CheckStateRole).toInt());
				if (state == Qt::Checked) {
					service = it.value()->child(i)->data(ServiceItem::ClassNameRole);
					break;
				}
			}
			selected.insert(it.key(),service);
		}
		group.setValue("list", selected);
		group.sync();
		Notifications::sendNotification(tr("To take effect you must restart qutIM"));
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

}

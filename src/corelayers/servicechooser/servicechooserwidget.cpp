/****************************************************************************
 *  servicechooserwidget.cpp
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

#include "servicechooserwidget.h"
#include "ui_servicechooserwidget.h"
#include <QStandardItem>
#include <libqutim/extensioninfo.h>
#include <libqutim/debug.h>
#include <libqutim/icon.h>
#include <QStringBuilder>
#include "servicedelegate.h"
#include "serviceitem.h"
#include <libqutim/configbase.h>

namespace Core
{
	ServiceChooserWidget::ServiceChooserWidget() :
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
		
		ExtensionInfoList exts = extensionList();
		for (int i = 0; i < exts.size(); i++) {
			const ExtensionInfo &info = exts.at(i);
			const char *serviceName = metaInfo(info.generator()->metaObject(), "Service");
			
			if (serviceName && *serviceName) {
				if (!m_service_items.contains(serviceName)) {
					ServiceItem *item = new ServiceItem(Icon(serviceIcon(serviceName)),serviceName,true);
					item->setData(true,Qt::UserRole);
					m_model->appendRow(item);
					m_service_items.insert(serviceName,item);
				}
			QIcon icon = !info.icon().name().isEmpty() ? info.icon() : Icon("help-hint");
			ServiceItem *item = new ServiceItem(icon,info.name());

			item->setToolTip(html(info));
			item->setCheckable(true);
			if (selected.value(serviceName).toString() == className(info))
				item->setCheckState(Qt::Checked);
			item->setData(false,Qt::UserRole);
			item->setServiceClassName(className(info));
			
			QList <QStandardItem *> items; //workaround
			items.append(item);
			
			m_service_items.value(serviceName)->appendRows(items);
			}
		}
		ui->treeView->expandAll();
	}
	void ServiceChooserWidget::cancelImpl()
	{

	}
	void ServiceChooserWidget::saveImpl()
	{
		ConfigGroup group = Config().group("services");
		QVariantMap selected;
		QHash<const char *, ServiceItem *>::const_iterator it;
		for (it = m_service_items.constBegin();it!=m_service_items.constEnd();it++) {
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
	}

	QString ServiceChooserWidget::html(const qutim_sdk_0_3::ExtensionInfo& info)
	{
		QString html = tr("<b>Name: </b> %1 <br />").arg(info.name());
		html += tr("<b>Description: </b> %1 <br />").arg(info.description());
		
		html += "<blockoute>";
		foreach (const PersonInfo &person, info.authors()) {
			html += tr("<b>Name:</b> %1</br>").arg(person.name());
			html += tr("<b>Task:</b> %1</br>").arg(person.task());
			html += tr("<b>Email:</b> <a href=\"mailto:%1\">%1</a></br>").arg(person.email());
			html += tr("<b>Webpage:</b> <a href=\"%1\">%1</a></br>").arg(person.web());
		}
		html += "</blockoute>";
		return html;
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

}

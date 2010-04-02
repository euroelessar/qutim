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
#include <libqutim/configbase.h>

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
				m_model->appendRow(item);
				m_protocol_items.insert(name,item);
			}
			QIcon icon = Icon("help-hint");
			//TODO Make normal names for the implementation of protocols			
			ServiceItem *item = new ServiceItem(icon,className(info));
			//ServiceItem *item = new ServiceItem(icon,info.name());

			item->setToolTip(html(info));
			item->setCheckable(true);
			if (selected.value(name).toString() == className(info))
				item->setCheckState(Qt::Checked);
			item->setData(false,Qt::UserRole);
			item->setServiceClassName(className(info));
			
			QList <QStandardItem *> items; //workaround
			items.append(item);
			
			m_protocol_items.value(name)->appendRows(items);
			}
		ui->treeView->expandAll();
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
	}

	QString ProtocolChooserWidget::html(const qutim_sdk_0_3::ExtensionInfo& info)
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

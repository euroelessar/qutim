/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Nikita Belov <null@deltaz.org>
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

#include "pluginchooserwidget.h"
#include "ui_servicechooserwidget.h"
#include <QStandardItem>
#include <qutim/extensioninfo.h>
#include <qutim/debug.h>
#include <qutim/icon.h>
#include <QStringBuilder>
#include <qutim/itemdelegate.h>
#include "serviceitem.h"
#include "servicechooser.h"
#include <qutim/configbase.h>
#include <qutim/notification.h>
#include <qutim/plugin.h>
#include <qutim/protocol.h>
#include <QRegExp>

namespace Core
{
PluginChooserWidget::PluginChooserWidget() :
	ui(new Ui::ServiceChooser),
	m_model(new QStandardItemModel)
{
	ui->setupUi(this);
	m_proxymodel = new SimpleFilterProxyModel(this);
	m_proxymodel->setSourceModel(m_model);
	m_proxymodel->setFilterKeyColumn(-1);
	m_proxymodel->setFilterCaseSensitivity(Qt::CaseInsensitive);
	ui->treeView->setModel(m_proxymodel);
	ui->treeView->setItemDelegate(new ItemDelegate(ui->treeView));
	ui->treeView->setIndentation(0);

	connect(m_model, SIGNAL(itemChanged(QStandardItem*)), SLOT(onItemChanged(QStandardItem*)));
	connect(ui->search, SIGNAL(textChanged(QString)), this, SLOT(filterPlugins(QString)));
#ifdef Q_WS_S60
	connect(ui->treeView, SIGNAL(clicked(QModelIndex)), SLOT(onItemClicked(QModelIndex)));
#endif
}
PluginChooserWidget::~PluginChooserWidget()
{
	delete ui;
}

void PluginChooserWidget::loadImpl()
{
	clear();
	Config group = Config().group("plugins/list");
	QStandardItem *parent_item = m_model->invisibleRootItem();

    QList<QWeakPointer<Plugin> > plugins = pluginsList();
	QStringList helper;
    for (int i = 0; i < plugins.size(); i++) {
        const PluginInfo &info = plugins.at(i).data()->info();
        QLatin1String class_name(plugins.at(i).data()->metaObject()->className());
        if (!m_plugin_items.contains(info.name())) {
			QIcon icon = info.icon();
			if (icon.isNull() || !icon.availableSizes().count())
				icon = Icon("applications-system");
			QString name = info.name();
			int index = qLowerBound(helper, name) - helper.constBegin();
			helper.insert(index, name);
			ServiceItem *item = new ServiceItem(icon, name);
			item->setToolTip(html(info));
			item->setCheckable(true);
			item->setData(true,ServiceItem::ExclusiveRole);
			item->setData(info.description().toString(), DescriptionRole);
			item->setCheckState((group.value(class_name, true) ? Qt::Checked : Qt::Unchecked));
			parent_item->insertRow(index, item);
			m_plugin_items.insert(class_name, item);
            m_plugins.insert(class_name, plugins.at(i).data());
		}
	}
}
void PluginChooserWidget::cancelImpl()
{

}
void PluginChooserWidget::saveImpl()
{
	Config group = Config().group("plugins/list");
	QHash<QString, ServiceItem *>::const_iterator it;
	bool needRestart = false;
	for (it = m_plugin_items.constBegin();it!=m_plugin_items.constEnd();++it)
	{
		bool oldValue = group.value(it.key(), true);
		bool newValue = (it.value()->checkState() == Qt::Checked ? true : false);

		group.setValue(it.key(), newValue);

		if ( oldValue && !newValue )
		{
			if( m_plugins.value(it.key())->avaiableExtensions().count() )
				needRestart = true;
			else
				m_plugins.value(it.key())->unload();
		}
		else if ( !oldValue && newValue )
		{
			if( m_plugins.value(it.key())->avaiableExtensions().count() )
				needRestart = true;
			else
				m_plugins.value(it.key())->load();
		}
	}
	if (needRestart)
		Notification::send(tr("To take effect you must restart qutIM"));
}

void PluginChooserWidget::clear()
{
	m_model->clear();
	m_plugin_items.clear();
}

void PluginChooserWidget::onItemChanged(QStandardItem* )
{
	emit modifiedChanged(true);
}

QString PluginChooserWidget::html(const qutim_sdk_0_3::PluginInfo& info)
{
	QString html = tr("<b>Name: </b> %1 <br />").arg(info.name());
	html += tr("<b>Description: </b> %1 <br />").arg(info.description());

	html += "<blockoute>";
	foreach (const PersonInfo &person, info.authors()) {
		html += "<br/>";
		html += tr("<b>Name:</b> %1<br/>").arg(person.name());
		if ( !person.task().toString().isEmpty() )
			html += tr("<b>Task:</b> %1<br/>").arg(person.task());
		if ( !person.email().isEmpty() )
			html += tr("<b>Email:</b> <a href=\"mailto:%1\">%1</a><br/>").arg(person.email());
		if ( !person.web().isEmpty() )
			html += tr("<b>Webpage:</b> <a href=\"%1\">%1</a><br/>").arg(person.web());
	}
	html += "</blockoute>";
	return html;
}

void PluginChooserWidget::onItemClicked(QModelIndex index)
{
	QStandardItem *item = m_model->itemFromIndex(index);
	if (item) {
		item->setCheckState(item->checkState() == Qt::Unchecked ? Qt::Checked : Qt::Unchecked);
	}
}

void PluginChooserWidget::filterPlugins(const QString& pluginname)
{
	m_proxymodel->setFilterWildcard(pluginname);
}

}


/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Alexander Kazarin <boiler@co.ru>
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

#include "sqlengine.h"
#include "historywindow.h"
#include "historysettings.h"
#include <qutim/iconmanagerinterface.h>

namespace SqlHistoryNamespace {

SqlEngine::SqlEngine()
{
	m_settings_widget = 0;
	m_save_history = m_show_recent_messages = true;
	m_max_num = 4;
}

bool SqlEngine::init(PluginSystemInterface *)
{
	m_name = "sql";
	m_version = "0.1.0";
	return true;
}

void SqlEngine::release()
{
	m_history_hash.clear();
	db.close();
}

void SqlEngine::setProfileName(const QString &profile_name)
{
	SystemsCity::instance().setProfileName(profile_name);

	QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/qutim."+SystemsCity::ProfileName(), "sqlhistory");
	m_save_history = settings.value("save", true).toBool();
	m_show_recent_messages = settings.value("showrecent", true).toBool();
	m_max_num = settings.value("recentcount", 4).toUInt();

	QString createtable_query;
	if (settings.value("sqlengine").toString()=="sqlite") {
		QString dbpath = SystemsCity::PluginSystem()->getProfilePath();
		if(dbpath.endsWith(QDir::separator())) dbpath.chop(1);
		dbpath += QDir::separator();
		dbpath += "history.db";

		db = QSqlDatabase::addDatabase("QSQLITE");
		db.setDatabaseName(dbpath);
		createtable_query = "create table history (id integer primary key autoincrement,"
			   "datetime int(16),"
			   "protocol varchar(16),"
			   "account varchar(64),"
			   "type int(1),"
			   "direction int(1),"
			   "itemname varchar(64),"
			   "text text )";
	}
	else if (settings.value("sqlengine").toString()=="mysql") {
		settings.beginGroup("mysql");
		db = QSqlDatabase::addDatabase("QMYSQL");
		db.setHostName(settings.value("host", "localhost").toString());
		db.setPort(settings.value("port", "3306").toInt());
		db.setUserName(settings.value("login").toString());
		db.setPassword(settings.value("password").toString());
		db.setDatabaseName(settings.value("dbname").toString());
		settings.endGroup();
		createtable_query = "create table if not exists `history` (`id` int(9) PRIMARY KEY auto_increment,"
			   "datetime int(16),"
			   "protocol varchar(16),"
			   "account varchar(64),"
			   "type int(1),"
			   "direction int(1),"
			   "itemname varchar(64),"
			   "text text );";
	}

	if (!db.open()) qDebug()<<"SQL History"<<"Database open error";

	if (!createtable_query.isEmpty()) {
		QSqlQuery query;
		query.exec(createtable_query);
	}

}

void SqlEngine::loadSettings()
{
}

void SqlEngine::setLayerInterface( LayerType type, LayerInterface *layer_interface)
{
	LayersCity::instance().setLayerInterface(type, layer_interface);
}

void SqlEngine::saveLayerSettings()
{
	if ( m_settings_widget )
		m_settings_widget->saveSettings();
	loadSettings();
}

QList<SettingsStructure> SqlEngine::getLayerSettingsList()
{
	m_settings.clear();
	if ( !m_settings_widget )
	{
		m_settings_widget = new SqlHistorySettings(SystemsCity::ProfileName());
		m_settings_item = new QTreeWidgetItem;
		m_settings_item->setText(0,QObject::tr("History"));
		m_settings_item->setIcon(0,Icon("history"));
		SettingsStructure tmp_struct;
		tmp_struct.settings_item = m_settings_item;
		tmp_struct.settings_widget = m_settings_widget;
		m_settings.append(tmp_struct);
	}
	return m_settings;
}

void SqlEngine::removeLayerSettings()
{
	if ( m_settings_widget )
	{
		delete m_settings_widget;
		m_settings_widget = 0;
		delete m_settings_item;
		m_settings_item = 0;
	}
}

void SqlEngine::openWindow(const TreeModelItem &item)
{
	QString identification = QString("%1.%2.%3").arg(item.m_protocol_name).arg(item.m_account_name).arg(item.m_item_name);
	if(!m_history_windows.value(identification))
	{
		TreeModelItem tmp = item;
		tmp.m_item_name = m_history_hash.value(identification, item.m_item_name);
		m_history_windows.insert(identification, QPointer<HistoryWindow>(new HistoryWindow(tmp, this)));
	}
}

uint SqlEngine::findEnd(QFile &file)
{
	return 0;
}

bool SqlEngine::storeMessage(const HistoryItem &item)
{
	if (!m_save_history) return false;

	QSqlQuery query;
	query.prepare("insert into history (datetime, protocol, account, type, direction, itemname, text) "
				  "values (?, ?, ?, ?, ?, ?, ?);");

	query.addBindValue(item.m_time.toTime_t());
	query.addBindValue(item.m_user.m_protocol_name);
	query.addBindValue(item.m_user.m_account_name);
	query.addBindValue(item.m_type);
	query.addBindValue(item.m_in ? 1 : 0);
	query.addBindValue(item.m_from);
	query.addBindValue(item.m_message);

	if (query.exec()) return true;
	else {
		qDebug()<<"SQL History"<<"insert error";
	}

	return false;
}

QList<HistoryItem> SqlEngine::getMessages(const TreeModelItem &item, const QDateTime &last_time)
{
	QList<HistoryItem> items;
	if (!m_show_recent_messages) return items;

	QString strquery ="select datetime, protocol, account, type, direction, itemname, text from history";
			strquery += " where datetime < "+QString::number(last_time.toTime_t());
			strquery += " and protocol='"+item.m_protocol_name+"'";
			strquery += " and account='"+item.m_account_name+"'";
			strquery += " and itemname='"+item.m_item_name+"'";
			strquery += " order by datetime desc limit "+QString::number(m_max_num);
	QSqlQuery query(strquery);

	while (query.next()) {
		HistoryItem hitem;
		hitem.m_user = item;
		hitem.m_time = QDateTime::fromTime_t(query.value(0).toUInt());
		hitem.m_from = query.value(5).toString();
		hitem.m_in = (query.value(4).toUInt()==0) ? false : true;
		hitem.m_message =  query.value(6).toString();
		items.prepend(hitem);
	}

	return items;
}

QString SqlEngine::quote(const QString &str)
{
	QString result = str;
	result.replace("'", "\\'");
	return result;
}

QString SqlEngine::unquote(const QString &str)
{
	QString result;
	return result;
}

QString SqlEngine::getFileName(const HistoryItem &item) const
{
	QString file;
	return file;
}

QDir SqlEngine::getAccountDir(const TreeModelItem &item) const
{
	return QDir();
}

void SqlEngine::setHistory(const TreeModelItem &item)
{

}

}


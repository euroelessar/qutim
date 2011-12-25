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

#include "webhistory.h"
#include <QScriptEngine>

bool webhistoryPlugin::init(PluginSystemInterface *plugin_system)
{
	qRegisterMetaType<TreeModelItem>("TreeModelItem");

	PluginInterface::init(plugin_system);
	m_plugin_icon = new QIcon(":/icons/webhistory-plugin.png");
	m_plugin_system = plugin_system;

	eventid_receive = m_plugin_system->registerEventHandler("Core/ChatWindow/ReceiveLevel2", this);
	eventid_send = m_plugin_system->registerEventHandler("Core/ChatWindow/SendLevel2", this);

	return true;
}

void webhistoryPlugin::release()
{
	if (db.isOpen()) db.close();
}

void webhistoryPlugin::loadSettings()
{
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "qutim/qutim."+m_profile_name, "plugin_webhistory");
	url.setUrl(settings.value("url").toString());


	QString login = settings.value("login").toString();
	QString password = settings.value("password").toString();

	if (!login.isNull() && !password.isNull())
		netreq.setRawHeader("Authorization", "Basic " +
			QByteArray(QString("%1:%2").arg(login).arg(password).toAscii()).toBase64());

	lasttime = settings.value("lasttime",1).toUInt();
	QString ts = QString::number(lasttime);
	if (!settings.value("nogethistory").toBool() && lasttime>0) {
		netreq.setUrl(QUrl(url.toString()+"?ts="+ts));
		netman->get(netreq);
		appended_count_json=appended_count_sqlite=0;
	}

	if (settings.value("store/json").toBool()) {
		db_filepath = SystemsCity::PluginSystem()->getProfilePath()+"/history.db";
		if (!QFile::exists(db_filepath)) db_filepath.clear();
	}
}

void webhistoryPlugin::processEvent(Event &event)
{
	if ( event.id == eventid_send) {
		TreeModelItem eventitem = *(TreeModelItem*)(event.args.at(0));
		QString msg = *(QString*)(event.args.at(1));
		insertHistory(0, msg, eventitem);
	}
	else if ( event.id == eventid_receive) {
		TreeModelItem eventitem = *(TreeModelItem*)(event.args.at(0));
		QString msg = *(QString*)(event.args.at(1));
		insertHistory(1, msg, eventitem);
	}
}

QWidget *webhistoryPlugin::settingsWidget()
{
	settingswidget = new historySettings(m_profile_name);
	connect(settingswidget, SIGNAL(settingsChanged()), this, SLOT(loadSettings()));
	return settingswidget;
}

void webhistoryPlugin::setProfileName(const QString &profile_name)
{
	m_profile_name = profile_name;

	netman = new QNetworkAccessManager();
	connect(netman, SIGNAL(finished(QNetworkReply*)), this, SLOT(netmanFinished(QNetworkReply*)));
	connect(netman, SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)), this, SLOT(netmanSslErrors(QNetworkReply*,QList<QSslError>)));

	SystemsCity::instance().setProfileName(profile_name);
	QString path = SystemsCity::PluginSystem()->getProfilePath();
	if(path.endsWith(QDir::separator()))
		path.chop(1);
	path += QDir::separator();
	path += "history";
	m_history_path = path;
	m_history_dir = path;

//qDebug()<<"History Path"<<path;

	loadSettings();

	if (!db_filepath.isEmpty()) {
		db = QSqlDatabase::addDatabase("QSQLITE");
		db.setDatabaseName(db_filepath);
		db.open();
	}
}

QString webhistoryPlugin::name()
{
	return "WebHistory";
}

QString webhistoryPlugin::description()
{
	return "Get/store history from/to web-server";
}

QIcon *webhistoryPlugin::icon()
{
	return m_plugin_icon;
}

QString webhistoryPlugin::type()
{
	return "other";
}

void webhistoryPlugin::removeSettingsWidget()
{
	delete settingsWidget();
}

void webhistoryPlugin::saveSettings()
{
	settingswidget->saveSettings();
}

void webhistoryPlugin::insertHistory(int direction, QString message, TreeModelItem item)
{
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "qutim/qutim."+m_profile_name, "plugin_webhistory");

	if (!settings.value("nostorehistory").toBool()) {
		QStringList postlist;
		postlist.append("direction="+QString::number(direction));
		postlist.append("protocol="+item.m_protocol_name);
		postlist.append("account="+item.m_account_name);
		postlist.append("itemname="+item.m_item_name);
		postlist.append("message="+message.toUtf8());
		postlist.append("ts="+QString::number(QDateTime::currentDateTime().toTime_t()));
		QByteArray postdata = postlist.join("&").toAscii();
		netreq.setUrl(QUrl(url));
		netman->post(netreq, postdata);
	}

	if (!settings.value("nolasttimeupdate").toBool()) {
		settings.setValue("lasttime", QDateTime::currentDateTime().toTime_t());
	}
}

void webhistoryPlugin::netmanFinished(QNetworkReply* reply)
{
	QString page = QString::fromUtf8(reply->readAll());

	if (reply->error() != 0) qDebug()<<"WebHistory Error"<<reply->error()<<page;
	else {
		QSettings settings(QSettings::IniFormat, QSettings::UserScope, "qutim/qutim."+m_profile_name, "plugin_webhistory");

		if (!reply->request().url().queryItemValue("ts").isEmpty()) {
			QString jsonstr = page;
			QScriptEngine sc_engine;
			QScriptValue sc_data = sc_engine.evaluate(page);

			int i=0;
			while (sc_data.property(i).property(0).isValid()) {
				QScriptValue sc_item = sc_data.property(i);
				QString h_protocol = sc_item.property(1).toString();
				QString h_account = sc_item.property(2).toString();

				HistoryItem hitem;
				hitem.m_type = 1;
				hitem.m_time = QDateTime::fromTime_t(sc_item.property(0).toUInt32());
				hitem.m_in = (sc_item.property(4).toUInt16()==1) ? true : false;
				hitem.m_from = sc_item.property(3).toString();
				hitem.m_message = sc_item.property(5).toString();
				hitem.m_user.m_item_name = sc_item.property(2).toString();
				hitem.m_user.m_protocol_name = h_protocol;
				hitem.m_user.m_account_name = h_account;

				if (!m_history_dir.exists(quote(h_protocol)+"."+quote(h_account))) {
					m_history_dir.mkdir(quote(h_protocol)+"."+quote(h_account));
				}

				QString hfilepath = m_history_path+"/"+quote(h_protocol)+"."+quote(h_account)+"/"+quote(hitem.m_from)+hitem.m_time.toString(".yyyyMM.'json'");
				QFile hfile(hfilepath);

				if (settings.value("store/json").toBool()) appendHistoryJSON(hfile, hitem);
				if (settings.value("store/sqlite").toBool()) appendHistorySQLite(hitem);

				i++;
			}

			if (lasttime > settings.value("lasttime").toUInt()) settings.setValue("lasttime", lasttime);

			if (settings.value("notify",true).toBool() && (appended_count_json>0 || appended_count_sqlite>0)) {
				QString nmessage = tr("Store messages from WebHistory:<br/>");
				if (appended_count_json>0) nmessage += tr("in JSON: %1<br/>").arg(appended_count_json);
				if (appended_count_sqlite>0) nmessage += tr("in SQLite: %1<br/>").arg(appended_count_sqlite);
				TreeModelItem nitem;
				nitem.m_item_name = "WebHistory";
				m_plugin_system->systemNotification(nitem, nmessage);
			}
//qDebug()<<"WebHistory get data"<<i;
		}
	}

}

void webhistoryPlugin::appendHistoryJSON(QFile &file, const HistoryItem &item)
{
	if (!file.open(QIODevice::ReadWrite | QIODevice::Text)) qDebug()<<"WebHistory"<<"file open failed"<<file.fileName();
	else {
		QByteArray data = file.readAll();
		QString strdata(data);
		bool skip=false;
		if (strdata.length()>0) {
			QRegExp rxdt("\"datetime\"\\s*\\:\\s*\"([^\"]+)\"");
			if (rxdt.lastIndexIn(strdata)>-1) {
				QDateTime filelastdatetime = QDateTime::fromString(rxdt.cap(1), Qt::ISODate);
				if (item.m_time.toTime_t() <= filelastdatetime.toTime_t()) skip=true;
			}
			if (!skip) {
				QRegExp rx("\\]\\s*$");
				int pos = (rx.indexIn(strdata)-1);
				if (pos>0) {
					file.resize(pos);
					file.seek(pos);
					if (pos > 5 /* for example :) */) file.write(",");
				}
				else {
					skip=true;
				}
			}
		}
		else {
			file.write("[");
		}

		if (!skip) {
			file.write("\n {\n");
			file.write("  \"datetime\": \""+item.m_time.toString(Qt::ISODate).toAscii()+"\",\n");
			file.write("  \"type\": "+QString::number(item.m_type).toLatin1()+",\n");
			file.write("  \"in\": ");	file.write(item.m_in ? "true" : "false"); file.write(",\n");
			file.write("  \"text\": "+json_quote(item.m_message).replace("\\n", "<br/>").toUtf8() +"\n");
			file.write(" }");
			file.write("\n]");
			appended_count_json++;
			if (item.m_time.toTime_t()>lasttime) lasttime = item.m_time.toTime_t();
//qDebug()<<"WebHistory append"<<file.fileName()<<item.m_time.toString(Qt::ISODate);
	}
		file.close();
	}
}

QString webhistoryPlugin::quote(const QString &str)
{
	const static bool true_chars[128] =
	{// 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, A, B, C, D, E, F
/* 0 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* 1 */ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
/* 2 */ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
/* 3 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0,
/* 4 */ 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
/* 5 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0,
/* 6 */ 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
/* 7 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0
	};
	QString result;
	result.reserve(str.size() * 5); // the worst variant
	const QChar *s = str.data();
	while(!s->isNull())
	{
		if(s->unicode() < 0xff && true_chars[s->unicode()])
			result += *s;
		else
		{
			result += '%';
			if(s->unicode() < 0x1000)
				result += '0';
			if(s->unicode() < 0x100)
				result += '0';
			if(s->unicode() < 0x10)
				result += '0';
			result += QString::number(s->unicode(), 16);
		}
		s++;
	}
	return result;
}

QString webhistoryPlugin::json_quote(const QString &str) {
  int len = str.length(), c;
  QString res('"'); res.reserve(len+128);
  for (int f = 0; f < len; f++) {
	QChar ch(str[f]);
	ushort uc = ch.unicode();
	if (uc < 32) {
	  // control char
	  switch (uc) {
		case '\b': res += "\\b"; break;
		case '\f': res += "\\f"; break;
		case '\n': res += "\\n"; break;
		case '\r': res += "\\r"; break;
		case '\t': res += "\\t"; break;
		default:
		  res += "\\u";
		  for (c = 4; c > 0; c--) {
			ushort n = (uc>>12)&0x0f;
			n += '0'+(n>9?7:0);
			res += (uchar)n;
		  }
		  break;
	  }
	} else {
	  // normal char
	  switch (uc) {
		case '"': res += "\\\""; break;
		case '\\': res += "\\\\"; break;
		default: res += ch; break;
	  }
	}
  }
  res += '"';
  return res;
}

void webhistoryPlugin::appendHistorySQLite(const HistoryItem &item) {
	if (!db.isOpen()) return;

	QSqlQuery query1("select datetime from history where protocol='"+item.m_user.m_protocol_name+"' and account='"+item.m_user.m_account_name+"' and itemname='"+item.m_from+"' and datetime>="+QString::number(item.m_time.toTime_t()));
	if (query1.next()) {
		return;
	}

	QSqlQuery query;
	query.prepare("insert into history (datetime, protocol, account, type, direction, itemname, text) "
				  "values (?, ?, ?, ?, ?, ?, ?)");

	query.addBindValue(item.m_time.toTime_t());
	query.addBindValue(item.m_user.m_protocol_name);
	query.addBindValue(item.m_user.m_account_name);
	query.addBindValue(item.m_type);
	query.addBindValue(item.m_in ? 1 : 0);
	query.addBindValue(item.m_from);
	query.addBindValue(item.m_message);

	if (query.exec()) appended_count_sqlite++;
}



Q_EXPORT_PLUGIN2(webhistoryPlugin,webhistoryPlugin);


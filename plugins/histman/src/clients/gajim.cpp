/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#include "gajim.h"
#include <QFileInfo>
#include <QDir>
#include <QTextDocument>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QCoreApplication>
#include <qutim/icon.h>

using namespace qutim_sdk_0_3;

namespace HistoryManager {

gajim::gajim()
{
}

void gajim::loadMessages(const QString &path)
{
	QDir dir = path;
	QFileInfo info(dir.filePath("logs.db"));
	if(!info.exists())
		return;
	QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
	db.setDatabaseName(info.absoluteFilePath());
	if(!db.open())
		return;
	setProtocol("jabber");
	setAccount(m_account);
	QSqlQuery jid_query("SELECT jid_id, jid FROM jids", db);
	setMaxValue(jid_query.size());
	int num = 0;
	while(jid_query.next())
	{
		QString jid_id = jid_query.value(0).toString();
		QString jid = jid_query.value(1).toString();
		setContact(jid);
		static QString query_str = "SELECT time, message, kind FROM logs "
								   "WHERE jid_id = %1 AND (kind = 4 OR kind = 6) "
								   "ORDER BY time ASC";
		QSqlQuery query(query_str.arg(jid_id), db);
		while(query.next())
		{
			Message message;
			message.setTime(QDateTime::fromTime_t(query.value(0).toInt()));
			message.setIncoming(query.value(2).toInt() == 4);
			message.setText(query.value(1).toString());
			appendMessage(message);
		}
		setValue(++num);
	}
}

bool gajim::validate(const QString &path)
{
	QDir dir = path;
	QFileInfo info(dir.filePath("logs.db"));
	return info.exists();
}

QString gajim::name()
{
	return "Gajim";
}

QIcon gajim::icon()
{
	return Icon("gajim");
}

QList<ConfigWidget> gajim::config()
{
	return QList<ConfigWidget>() << (m_config = createAccountWidget("Jabber"));
}

bool gajim::useConfig()
{
	m_account = m_config.second->property("currentText").toString();
	return true;
}

QString gajim::additionalInfo()
{
	return QCoreApplication::translate("ClientConfigPage", "Select your Jabber account.");
}

}


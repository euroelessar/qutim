/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2012 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#include "webkitpreviewloader.h"
#include "webkitpreviewunits_p.h"
#include <QVariantMap>
#include <QFileInfo>
#include <QDir>
#include <qutim/config.h>
#include <QTextDocument>

using namespace qutim_sdk_0_3;

WebKitPreviewLoader::WebKitPreviewLoader()
{
}

WebKitPreview::Ptr WebKitPreviewLoader::loadPreview(const QString &fileName)
{
	WebKitPreview::Ptr preview = WebKitPreview::Ptr::create();
	WebKitPreviewSession *session = new WebKitPreviewSession();
	WebKitPreviewProtocol *protocol = new WebKitPreviewProtocol(session);
	QMap<QString, ChatUnit*> units;
	Account *account = 0;
	preview->session.reset(session);
	QString timeFormat = QLatin1String("yyyy-MM-dd hh:mm:ss -0500");
	Config cfg(fileName);
	QDir dir = QFileInfo(fileName).absoluteDir();
	QString accountId;
	QVariantMap previewData = cfg.rootValue().toMap();
	{
		QVariantMap data = previewData.value(QLatin1String("Chat")).toMap();
		session->setDateOpened(QDateTime::fromString(data.value(QLatin1String("Date Opened")).toString(),
		                                             timeFormat));
		accountId = data.value(QLatin1String("Source UID")).toString();
	}
	{
		QVariantList participants = previewData.value(QLatin1String("Participants")).toList();
		// Firstly find account
		for (int i = 0; i < participants.size(); ++i) {
			QVariantMap data = participants[i].toMap();
			if (data.value(QLatin1String("UID")).toString() == accountId) {
				data.insert(QLatin1String("UserIcon Name"),
				            dir.filePath(data.value(QLatin1String("UserIcon Name")).toString()));
				account = new WebKitPreviewAccount(data, protocol);
				break;
			}
		}
		// Then participants
		for (int i = 0; i < participants.size(); ++i) {
			QVariantMap data = participants[i].toMap();
			QString id = data.value(QLatin1String("UID")).toString();
			if (id != accountId) {
				data.insert(QLatin1String("UserIcon Name"),
				            dir.filePath(data.value(QLatin1String("UserIcon Name")).toString()));
				units.insert(id, new WebKitPreviewChatUnit(data, account));
				break;
			}
		}
		session->setChatUnit(units.begin().value());
	}
	{
		QVariantList messages = previewData.value(QLatin1String("Preview Messages")).toList();
		for (int i = 0; i < messages.size(); ++i) {
			QVariantMap data = messages[i].toMap();
			Message msg;
			msg.setProperty("autoreply", data.value(QLatin1String("Autoreply"), true).toBool());
			msg.setTime(QDateTime::fromString(data.value(QLatin1String("Date")).toString(), timeFormat));
			msg.setIncoming(!data.value(QLatin1String("Outgoing")).toBool());
			{
				QTextDocument doc;
				doc.setHtml(data.value(QLatin1String("Message")).toString());
				msg.setText(doc.toPlainText());
			}
			QString id = data.value(QLatin1String("From")).toString();
			msg.setChatUnit(units.value(id, session->unit()));
			QString type = data.value(QLatin1String("Type")).toString();
			if (type == QLatin1String("Status"))
				msg.setProperty("service", true);
			QString subtype = data.value("Status Message Type").toString();
			msg.setProperty("displayClasses", QStringList(subtype));
			msg.setProperty("history", msg.time() < session->dateOpened());
			preview->messages << msg;
		}
	}
	return preview;
}

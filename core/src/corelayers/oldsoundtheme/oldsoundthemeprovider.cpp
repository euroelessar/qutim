/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#include "oldsoundthemeprovider.h"
#include <QDebug>
#include <QStringList>
#include <QDomDocument>
#include <QFile>
#include <QDir>

namespace Core
{
OldSoundThemeProvider::OldSoundThemeProvider(const QString &name, const QString &path, QString variant)
{
	m_themeName = name;
	const Notification::Type xmlEventTypes[] = {
		Notification::IncomingMessage,
		Notification::OutgoingMessage,
		Notification::AppStartup,
		Notification::BlockedMessage,
		Notification::ChatUserJoined,
		Notification::ChatUserLeft,
		Notification::ChatIncomingMessage,
		Notification::ChatOutgoingMessage,
		Notification::FileTransferCompleted,
		Notification::UserOnline,
		Notification::UserOffline,
		Notification::UserChangedStatus,
		Notification::UserHasBirthday,
		Notification::UserTyping,
		Notification::System,
		Notification::Attention,
	    // And again some of them for legacy names
		Notification::UserOnline,
		Notification::UserOffline,
		Notification::UserChangedStatus,
		Notification::UserHasBirthday,
		Notification::AppStartup,
		Notification::IncomingMessage,
		Notification::ChatIncomingMessage,
		Notification::OutgoingMessage,
		Notification::ChatOutgoingMessage,
		Notification::System,
		Notification::UserTyping,
		Notification::BlockedMessage
	};
	const char* const xmlEventNames[] = {
		"IncomingMessage",
		"OutgoingMessage",
		"AppStartup",
		"BlockedMessage",
		"ChatUserJoined",
		"ChatUserLeft",
		"ChatIncomingMessage",
		"ChatOutgoingMessage",
		"FileTransferCompleted",
		"UserOnline",
		"UserOffline",
		"UserChangedStatus",
		"UserHasBirthday",
		"UserTyping",
		"System",
		"Attention",
		"c_online",
		"c_offline",
		"c_changed_status",
		"c_birth",
		"start",
		"m_get",
		"m_chat_get",
		"m_send",
		"m_chat_send",
		"sys_event",
		"c_typing",
		"c_blocked_message"
	};
	const int eventsCount = sizeof(xmlEventTypes) / sizeof(xmlEventTypes[0]);

	Q_ASSERT(sizeof(xmlEventTypes) / sizeof(xmlEventTypes[0]) == sizeof(xmlEventNames) / sizeof(xmlEventNames[0]));

	QDir dir(path);
	if (variant.isEmpty())
		variant = dir.entryList(QStringList("*.xml"), QDir::Files).value(0);
	else
		variant += ".xml";
	QFile file(dir.filePath(variant));
	if (file.open(QIODevice::ReadOnly)) {
		QDomDocument doc;
		doc.setContent(&file);
		if (doc.doctype().name() != QLatin1String("qutimsounds"))
			return;
		QDomElement rootElement = doc.documentElement();
		QDomNodeList soundsNodeList = rootElement.elementsByTagName("sounds");
		if (soundsNodeList.count() != 1)
			return;
		QDomElement soundsElement = soundsNodeList.at(0).toElement();
		soundsNodeList = soundsElement.elementsByTagName("sound");
		QDomElement soundElement;
		QString eventName, soundFileName;

		for (int i = 0; i < soundsNodeList.count(); i++) {
			soundElement = soundsNodeList.at(i).toElement();
			eventName = soundElement.attribute("event");
			if (eventName.isEmpty() || !soundElement.elementsByTagName("file").count())
				continue;
			soundFileName = dir.filePath(soundElement.elementsByTagName("file").at(0).toElement().text());
			if (!QFile::exists(soundFileName)) 
				continue;
			for (int i = 0; i < eventsCount; i++) {
				if (eventName == QLatin1String(xmlEventNames[i])) {
					m_map.insert(xmlEventTypes[i], soundFileName);
					break;
				}
			}
		}
		m_filePath = file.fileName();
	}
}

bool OldSoundThemeProvider::setSoundPath(Notification::Type sound, const QString &file)
{
	m_map.insert(sound, file);
	return true;
}

QString OldSoundThemeProvider::soundPath(Notification::Type sound)
{
	return m_map.value(sound);
}

QString OldSoundThemeProvider::themeName()
{
	return m_themeName;
}

bool OldSoundThemeProvider::saveTheme()
{
	// TODO: implement
	return false;
}
}


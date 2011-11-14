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

#include "chatunitadaptor.h"
#include "conferenceadaptor.h"
#include "buddyadapter.h"
#include "contactadaptor.h"
#include "accountadaptor.h"
#include <qutim/event.h>
#include <QCryptographicHash>

extern quint16 dbus_adaptor_event_id;

Q_GLOBAL_STATIC(ChatUnitPathHash, chatUnitHash)

const ChatUnitPathHash &ChatUnitAdaptor::hash()
{
	return *chatUnitHash();
}

QDBusObjectPath ChatUnitAdaptor::ensurePath(QDBusConnection dbus, ChatUnit *unit)
{
	QDBusObjectPath path = chatUnitHash()->value(unit);
	if (path.path().isEmpty() && unit) {
		QDBusObjectPath accountPath = AccountAdaptor::hash().value(unit->account());
		ChatUnitAdaptor *adaptor = new ChatUnitAdaptor(dbus, accountPath, unit);
		Event(dbus_adaptor_event_id, qVariantFromValue<ChatUnit*>(unit)).send();
		if (Conference *conf = qobject_cast<Conference*>(unit)) {
			new ConferenceAdaptor(dbus, conf);
		} else {
			if (Buddy *buddy = qobject_cast<Buddy*>(unit))
				new BuddyAdapter(buddy);
			if (Contact *contact = qobject_cast<Contact*>(unit))
				new ContactAdaptor(contact);
		}
		dbus.registerObject(adaptor->path().path(), unit, QDBusConnection::ExportAdaptors);
		path = adaptor->path();
	}
	return path.path().isEmpty() ? QDBusObjectPath("/") : path;
}

ChatUnitAdaptor::ChatUnitAdaptor(const QDBusConnection &dbus, const QDBusObjectPath &acc, ChatUnit *unit) :
		QDBusAbstractAdaptor(unit), m_chatUnit(unit), m_dbus(dbus), m_accountPath(acc)
{
	QString path = acc.path().replace(QLatin1String("Account"), QLatin1String("ChatUnit"));
	QCryptographicHash hash(QCryptographicHash::Md5);
	hash.addData(unit->id().toUtf8());
	path += QLatin1String("/");
	path += QLatin1String(hash.result().toHex());
	m_path = QDBusObjectPath(path);
	chatUnitHash()->insert(m_chatUnit, m_path);
	connect(unit, SIGNAL(titleChanged(QString,QString)), this, SIGNAL(titleChanged(QString,QString)));
}

ChatUnitAdaptor::~ChatUnitAdaptor()
{
	chatUnitHash()->remove(m_chatUnit);
}

QStringList ChatUnitAdaptor::lowerUnits()
{
	QStringList list;
	foreach (ChatUnit *unit, m_chatUnit->lowerUnits())
		list << unit->id();
	return list;
}

QDBusObjectPath ChatUnitAdaptor::upperUnit()
{
	return ChatUnitAdaptor::ensurePath(m_dbus, m_chatUnit->upperUnit());
}


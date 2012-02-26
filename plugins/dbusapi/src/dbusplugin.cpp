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

#include "dbusplugin.h"
#include "protocoladaptor.h"
#include "chatlayeradapter.h"
#include "chatunitadaptor.h"
#include <QApplication>
#include <QDBusError>
#include <QDBusArgument>
#include <QDBusMetaType>
#include <qutim/protocol.h>
#include <qutim/account.h>
#include <qutim/event.h>
#include <QDebug>

QDBusArgument &operator<<(QDBusArgument &argument, const Status &status)
{
	// At first time Qt calls this method with default-constructed
	// status to research type signature and some-thing else than
	// map's begind and end will discourage it. So lets fake it.
	static bool first = true;
	argument.beginMap(QVariant::String, qMetaTypeId<QDBusVariant>());
	if (first) {
		first = false;
	} else {
		argument.beginMapEntry();
		argument << QString::fromLatin1("type") << QDBusVariant(qint32(status.type()));
		argument.endMapEntry();
		
		argument.beginMapEntry();
		argument << QString::fromLatin1("name") << QDBusVariant(status.name().toString());
		argument.endMapEntry();
		
		argument.beginMapEntry();
		argument << QString::fromLatin1("text") << QDBusVariant(status.text());
		argument.endMapEntry();
		
//		argument.beginMapEntry();
//		argument << QLatin1String("extendedStatuses") << QDBusVariant(status.extendedStatuses());
//		argument.endMapEntry();
//		TODO: Implement Status::dynamicPropertyNames method
//		foreach (const QByteArray &prop, status.dynamicPropertyNames()) {
//			argument.beginMapEntry();
//			argument << QString::fromLatin1(prop, prop.size());
//			argument << QDBusVariant(status.property(prop));
//			argument.endMapEntry();
//		}
	}
	argument.endMap();
	return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, Status &status)
{
	argument.beginMap();
	QString key;
	QVariant value;
	while (!argument.atEnd()) {
		argument.beginMapEntry();
		argument >> key >> value;
		status.setProperty(key.toLatin1(), value);
		argument.endMapEntry();
	}
	argument.endMap();
	return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const Message &msg)
{
	static bool first = true;
	argument.beginMap(QVariant::String, qMetaTypeId<QDBusVariant>());
	if (first) {
		first = false;
	} else {
		argument.beginMapEntry();
		argument << QString::fromLatin1("time") << QDBusVariant(msg.time());
		argument.endMapEntry();
		
		argument.beginMapEntry();
		ChatUnit *unit = const_cast<ChatUnit*>(msg.chatUnit());
		QDBusObjectPath path = ChatUnitAdaptor::ensurePath(QDBusConnection::sessionBus(), unit);
		argument << QString::fromLatin1("chatUnit") << QDBusVariant(qVariantFromValue(path));
		argument.endMapEntry();
		
		argument.beginMapEntry();
		argument << QString::fromLatin1("text") << QDBusVariant(msg.text());
		argument.endMapEntry();
		
		argument.beginMapEntry();
		argument << QString::fromLatin1("incoming") << QDBusVariant(msg.isIncoming());
		argument.endMapEntry();
		
		foreach (const QByteArray &prop, msg.dynamicPropertyNames()) {
			argument.beginMapEntry();
			argument << QString::fromLatin1(prop, prop.size());
			argument << QDBusVariant(msg.property(prop));
			argument.endMapEntry();
		}
	}
	argument.endMap();
	return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, Message &msg)
{
	argument.beginMap();
	QString key;
	QVariant value;
	while (!argument.atEnd()) {
		argument.beginMapEntry();
		argument >> key >> value;
		msg.setProperty(key.toLatin1(), value);
		argument.endMapEntry();
	}
	argument.endMap();
	return argument;
}

quint16 dbus_adaptor_event_id = 0;

DBusPlugin::DBusPlugin() : m_dbus(0)
{
}

void DBusPlugin::init()
{
	addAuthor(QLatin1String("euroelessar"));
	setInfo(QT_TRANSLATE_NOOP("Plugin", "DBus API"),
			QT_TRANSLATE_NOOP("Plugin", "Added ability to control qutIM by DBus"),
			PLUGIN_VERSION(0, 0, 1, 0), ExtensionIcon("network-wireless"));
	setCapabilities(Loadable);
}

bool DBusPlugin::load()
{
	if (m_dbus)
		return false;
	m_dbus = new QDBusConnection(QDBusConnection::connectToBus(QDBusConnection::SessionBus, "qutim"));
	if (!m_dbus->registerService("org.qutim")) {
		qDebug() << "smth wrong at DBUS";
		delete m_dbus;
		m_dbus = 0;
		return false;
	}
	dbus_adaptor_event_id = Event::registerType("dbus-adaptors-request");
	qDBusRegisterMetaType<Status>();
	qDBusRegisterMetaType<Message>();
	qDBusRegisterMetaType<MessageList>();
	qDBusRegisterMetaType<QList<QDBusObjectPath> >();
	qDebug() << "all ok at DBUS";
	foreach (Protocol *proto, Protocol::all()) {
		ProtocolAdaptor *adaptor = new ProtocolAdaptor(*m_dbus, proto);
		if (adaptor->path().path().isEmpty())
			qWarning() << "proto path is empty" << proto->objectName();
		else if (!m_dbus->registerObject(adaptor->path().path(), proto, QDBusConnection::ExportAdaptors))
			qDebug() << m_dbus->lastError().message() << QDBusError::errorString(m_dbus->lastError().type());
	}
	new ChatLayerAdapter(*m_dbus);
	m_dbus->registerObject("/ChatLayer", ChatLayer::instance(), QDBusConnection::ExportAdaptors);
	m_dbus->registerObject("/App", qApp, QDBusConnection::ExportAllContents);
	return true;
}

bool DBusPlugin::unload()
{
	if (!m_dbus)
		return false;
	m_dbus->unregisterService("org.qutim");
	delete m_dbus;
	m_dbus = 0;
	return true;
}

QUTIM_EXPORT_PLUGIN(DBusPlugin)


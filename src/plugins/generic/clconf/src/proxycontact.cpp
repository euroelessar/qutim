/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Alexey Prokhin <alexey.prokhin@yandex.ru>
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

#include "proxycontact.h"
#include "proxyaccount.h"
#include "clconfplugin.h"
#include <qutim/tooltip.h>
#include <qutim/event.h>
#include <qutim/metaobjectbuilder.h>
#include <QApplication>

ProxyContact::ProxyContact(Conference *conf) :
	Contact(ClConfPlugin::instance()->account(conf->account())), m_conf(conf)
{
	connect(conf, SIGNAL(destroyed()), SLOT(deleteLater()));
	setMenuOwner(m_conf.data());
	setMenuFlags(ShowOwnerActions);
	m_conf.data()->installEventFilter(this);
	connect(m_conf.data(), SIGNAL(titleChanged(QString,QString)), SIGNAL(nameChanged(QString,QString)));
	connect(m_conf.data(), SIGNAL(titleChanged(QString,QString)), SIGNAL(titleChanged(QString,QString)));
	connect(m_conf.data(), SIGNAL(joinedChanged(bool)), SLOT(updateStatus()));
	connect(m_conf.data()->account(), SIGNAL(statusChanged(qutim_sdk_0_3::Status,qutim_sdk_0_3::Status)),
			SLOT(onAccountStatusChanged(qutim_sdk_0_3::Status,qutim_sdk_0_3::Status)));
	updateStatus();
}

QStringList ProxyContact::tags() const
{
	return QStringList() << QT_TRANSLATE_NOOP("ClConf", "Conferences");
}

void ProxyContact::setTags(const QStringList &tags)
{
	Q_UNUSED(tags);
}

bool ProxyContact::isInList() const
{
	return true;
}

void ProxyContact::setInList(bool inList)
{
	Q_UNUSED(inList);
}

QString ProxyContact::id() const
{
	return m_conf ? m_conf.data()->id() : QString();
}

QString ProxyContact::title() const
{
	return m_conf ? m_conf.data()->title() : QString();
}

QString ProxyContact::name() const
{
	return m_conf ? m_conf.data()->title() : QString();
}

Status ProxyContact::status() const
{
	return m_conf ? m_status : Status();
}

bool ProxyContact::sendMessage(const Message &message)
{
	return m_conf ? m_conf.data()->sendMessage(message) : false;
}

void ProxyContact::onAccountStatusChanged(const qutim_sdk_0_3::Status &current,
										  const qutim_sdk_0_3::Status &previous)
{
	if (previous.type() != Status::Offline && current.type() == Status::Offline)
		updateStatus();
	else if (previous.type() == Status::Offline && current.type() != Status::Offline)
		updateStatus();
}

void ProxyContact::updateStatus()
{
	Status previous = m_status;
	if (!m_conf || account()->status().type() == Status::Offline)
		m_status.setType(Status::Offline);
	else
		m_status.setType(m_conf.data()->isJoined() ? Status::Online : Status::DND);
	m_status.initIcon(QLatin1String("conference-") + MetaObjectBuilder::info(metaObject(), "Protocol"));
	emit statusChanged(m_status, previous);
}

bool ProxyContact::event(QEvent *ev)
{
	if (m_conf && ev->type() == ToolTipEvent::eventType())
		return qApp->sendEvent(m_conf.data(), ev);
	return Contact::event(ev);
}

bool ProxyContact::eventFilter(QObject *obj, QEvent *ev)
{
	if (obj == m_conf.data() && ev->type() == Event::eventType()) {
		Event *event = static_cast<Event*>(ev);
		static quint16 realUnitRequestEvent = Event::registerType("real-chatunit-request");
		if (event->id == realUnitRequestEvent) {
			event->args[0] = qVariantFromValue<Contact*>(this);
			event->accept();
			return true;
		}
	}
	return Contact::eventFilter(obj, ev);
}


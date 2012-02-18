/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Nikita Belov <null@deltaz.org>
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

#include "waccount.h"
#include "wcontact.h"
#include <qutim/servicemanager.h>

WContact::WContact(const QString &code, const QString &name, Account *account)
    : Contact(account)
{
	m_city = code;
	m_name = name;
	m_tags << QLatin1String("Weather");
	setMenuFlags(ShowSelfActions);

	m_status.setType(Status::Online);
	m_status.setIcon(QIcon(":/icons/weather.png"));

	addToList();
	QMetaObject::invokeMethod(ServiceManager::getByName("ContactList"),
	                          "addContact",
	                          Q_ARG(qutim_sdk_0_3::Contact*, this));
}

WContact::~WContact()
{
}

WAccount *WContact::account()
{
	return static_cast<WAccount*>(Contact::account());
}

bool WContact::event(QEvent *ev)
{
	if (ev->type() == ToolTipEvent::eventType()) {
		ToolTipEvent *event = static_cast< ToolTipEvent * >(ev);
		event->addField(QT_TRANSLATE_NOOP("Weather", "Weather"), m_status.text());
	}

	return Contact::event(ev);
}

void WContact::updateStatus()
{
	if (account()->getShowStatusRow()) {
		account()->update(this, false);
	} else {
		Status current = m_status;
		m_status.setText(QString());
		emit statusChanged(m_status, current);
	}
}

void WContact::getWeather()
{
	account()->update(this, true);
}

void WContact::getForecast()
{
	account()->getForecast(this);
}

bool WContact::sendMessage(const Message &)
{
	return false;
}

void WContact::setName(const QString &)
{
}

void WContact::setNameInternal(const QString &name)
{
	if (m_name == name)
		return;
	QString previous = m_name;
	m_name = name;
	emit nameChanged(m_name, previous);
}

void WContact::setTags(const QStringList &)
{
}

QString WContact::id() const
{
	return m_city;
}

QString WContact::name() const
{
	return m_name;
}

QString WContact::title() const
{
	return m_name;
}

QStringList WContact::tags() const
{
	return m_tags;
}

Status WContact::status() const
{
	return m_status;
}

void WContact::setStatusInternal(QString iconId, const QString &text)
{
	Status previous = m_status;
	m_status.setIcon(QIcon(QString::fromLatin1(":/icons/%1.png").arg(iconId)));
	m_status.setText(text);
	emit statusChanged(m_status, previous);
}

bool WContact::isInList() const
{
	return true;
}

void WContact::setInList(bool)
{
}

QString WContact::avatar() const
{
	return ":/icons/weather_big.png";
}


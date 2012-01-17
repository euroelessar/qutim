/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Belov Nikita <null@deltaz.org>
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

WAccount::WAccount(WProtocol *protocol) : Account("Weather", (Protocol *)protocol)
{
	settings = new GeneralSettingsItem< WSettings >(Settings::Plugin, QIcon(":/icons/weather.png"), QT_TRANSLATE_NOOP("Weather", "Weather"));
	Settings::registerItem(settings);

	connect(settings->widget(), SIGNAL(saved()), this, SLOT(loadSettings()));

	m_timer = new QTimer();
	connect(m_timer, SIGNAL(timeout()), this, SLOT(updateAll()));

	loadSettings();
}

WAccount::~WAccount()
{
	Settings::removeItem(settings);
	delete settings;
	foreach (WContact *v, m_contacts)
		v->deleteLater();
}

ChatUnit *WAccount::getUnitForSession(ChatUnit *unit)
{
	return unit;
}

ChatUnit *WAccount::getUnit(const QString &unitId, bool create)
{
	Q_UNUSED(unitId);
	Q_UNUSED(create);

	return 0;
}

QString WAccount::name() const
{
	return "You";
}

QString WAccount::getThemePath()
{
	return s_themePath;
}

bool WAccount::getShowStatusRow()
{
	return s_showStatusRow;
}

void WAccount::setStatus(Status status)
{
	Q_UNUSED(status);
}

void WAccount::loadSettings()
{
	Config mainGroup = Config("weather").group("main");

	m_timer->stop();
	m_timer->start(mainGroup.value("interval", 25) * 1000);

	s_showStatusRow = (mainGroup.value("showStatus", true) ? Qt::Checked : Qt::Unchecked);
	s_themePath = (mainGroup.value("useDefaultTheme", true) ? ":/themes/default/" : mainGroup.value("themePath", QString()));

	loadContacts();
}

void WAccount::updateAll()
{
	foreach (WContact *v, m_contacts)
		v->update();
}

void WAccount::loadContacts()
{
	Config config(QLatin1String("weather"));
	config.beginGroup(QLatin1String("main"));
	QHash<QString, WContact *> contacts;
	int count = config.beginArray(QLatin1String("contacts"));
	for (int i = 0; i < count; i++) {
		QString cityCode = config.value(QLatin1String("code"), QString());
		QString cityName = config.value(QLatin1String("name"), QString());
		WContact *cityContact = m_contacts.take(cityCode);
		
		if (!cityContact)
			cityContact = new WContact(cityCode, cityName, this);
		contacts.insert(cityCode, cityContact);
		cityContact->update();
	}

	qDeleteAll(m_contacts);
	m_contacts = contacts;
}

